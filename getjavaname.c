/**
 * getjavaname.c
 *
 * This program prints the class name of a java class file.
 *
 * It does so using as little memory as possible.
 *
 * @author michael-m-2983
 * @date 2025-03-11
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/**
 * Read a constant pool entry and advance the file pointer by its size.
 *
 * Essentially, discard all data about the entry.
 *
 */
void read_cp_entry(uint32_t index, FILE *fp) {
  uint8_t entry_type = 0;
  fread(&entry_type, sizeof(entry_type), 1, fp);

  if (entry_type == 0) {
    fprintf(stderr,
            "Failed to read constant pool entry type at index %d! (pos=%lx)\n",
            index, ftell(fp));
    exit(EXIT_FAILURE);
  }

  uint16_t utf8_length = 0; // for the utf8 case only

  switch (entry_type) {
  case 7:    // Class - 2 bytes of data
  case 8:    // String - 2 bytes of data
  case 16:   // MethodType - 2 bytes of data
  case 19:   // Module - 2 bytes of data
  case 20: { // Package - 2 bytes of data
    fseek(fp, 2, SEEK_CUR);
    break;
  }
  case 15: { // MethodHandle - 3 bytes of data
    fseek(fp, 3, SEEK_CUR);
    break;
  }
  case 9:    // Field ref - 4 bytes of data
  case 10:   // Method ref - 4 bytes of data
  case 11:   // Interface red - 4 bytes of data
  case 3:    // Integer - 4 bytes of data
  case 4:    // Foat - 4 bytes of data
  case 12:   // Nameandtype - 4 bytes of data
  case 14:   // Invokedynamic - 4 bytes of data
  case 17:   // Constant dynamic - 4 bytes of data
  case 18: { // Invokedynamic again - 4 bytes of data
    fseek(fp, 4, SEEK_CUR);
    break;
  }
  case 5:   // Long - 8 bytes of data
  case 6: { // Double - 8 bytes of data
    fseek(fp, 8, SEEK_CUR);
    break;
  }
  case 1: { // Utf8 - dynamic length
    fread(&utf8_length, sizeof(utf8_length), 1, fp);
    utf8_length = ntohs(utf8_length);
    fseek(fp, utf8_length, SEEK_CUR);
    break;
  }
  default: {
    fprintf(stderr,
            "Found an invalid constant pool type '%d' at cp_index %d (offset "
            "%lx).\n",
            entry_type, index, ftell(fp) - 1);
    exit(EXIT_FAILURE);
    break;
  }
  }
}

int main(int argc, char **argv) {
  if (argc != 2 || strncmp(argv[1], "-h", 2) == 0) {
    fprintf(stderr, "Usage: %s <file.class>\n", argv[0]);
    exit(1);
  }

  FILE *fp = fopen(argv[1], "rb");

  if (fp == NULL) {
    perror("Error while opening file");
    exit(1);
  }

  // Read the magic number of the classfile
  uint32_t magic_number = 0;
  fread(&magic_number, sizeof(magic_number), 1, fp);
  magic_number = ntohl(magic_number); // Java class files are big-endian always

  if (magic_number != 0xCAFEBABE) {
    fclose(fp);
    fprintf(stderr, "Error: Invalid class file magic number!\n");
    exit(1);
  }

  // Skip magic number and version info
  fseek(fp, 8, SEEK_SET);

  uint16_t cp_entry_count = 0;
  fread(&cp_entry_count, sizeof(cp_entry_count), 1, fp);
  cp_entry_count = ntohs(cp_entry_count);

  unsigned long *cp_offsets = calloc(cp_entry_count, sizeof(unsigned long));

  for (int i = 1; i < cp_entry_count; i++) {
    cp_offsets[i] = ftell(fp);
    read_cp_entry(i, fp);
  }

  // Skip the access flags, because they are not useful.
  fseek(fp, 2, SEEK_CUR);

  // Read the 'this_class' constant pool index.
  uint16_t cp_index_thisclass = 0;
  fread(&cp_index_thisclass, sizeof(cp_index_thisclass), 1, fp);
  cp_index_thisclass = ntohs(cp_index_thisclass);

  // Jump to the offset of the Class cp entry's name_index field
  fseek(fp, cp_offsets[cp_index_thisclass] + 1, SEEK_SET);

  // Read the index of the utf8 constant containing the class name
  uint16_t cp_name_index = 0;
  fread(&cp_name_index, sizeof(cp_name_index), 1, fp);
  cp_name_index = ntohs(cp_name_index);

  // Go to the UTF8 constant and skip the tag bit
  fseek(fp, cp_offsets[cp_name_index] + 1, SEEK_SET);

  // Free the constant pool offsets as they are no longer needed
  free(cp_offsets);

  // Read the length of the constant - this is in bytes, not chars
  uint16_t name_length = 0;
  fread(&name_length, sizeof(name_length), 1, fp);
  name_length = ntohs(name_length);

  // Read all bytes of the string into a buffer
  uint8_t *name_bytes = (uint8_t *)calloc(name_length, sizeof(uint8_t));
  if (fread(name_bytes, sizeof(uint8_t), name_length, fp) != name_length) {
    perror("Failed to get the class name: ");
    free(name_bytes);
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  // Print the name of the class
  for (int i = 0; i < name_length; i++) {
    printf("%c", name_bytes[i]);
  }
  printf("\n");

  free(name_bytes);
  fclose(fp);
  return EXIT_SUCCESS;
}
