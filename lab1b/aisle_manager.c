/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s):  Po Peng, Kevin Le
 * NetID(s): ericpp, kevin218
 *
 * ----------------------------------------------------------------------------
 * Overview
 * ----------------------------------------------------------------------------
 *  This is a program to keep track of the items in a small aisle of a store.
 *
 *  A store's aisle is represented by a 64-bit long integer, which is broken
 *  into 4 16-bit sections representing one type of item each. Note that since
 *  a section is 16-bits, it fits nicely into C's short datatype.
 *
 *  Aisle Layout:
 *
 *    Within an aisle, sections are indexed starting with the least-significant
 *    section being at index 0 and continuing up until one less than the number
 *    of sections.
 *
 *    Example aisle:
 *
 *                MSB                                                       LSB
 *                  +-------------+-------------+-------------+-------------+
 *                  |  Section 3  |  Section 2  |  Section 1  |  Section 0  |
 *                  +-------------+-------------+-------------+-------------+
 *                  |             |             |             |             |
 *      bit offset: 64            48            32            16            0
 *
 *  Section Layout:
 *
 *    A section in an aisle is broken into 2 parts. The 6 most significant bits
 *    represent a unique identifier for the type of item stored in that
 *    section. The rest of the bits in a section (10 least significant)
 *    indicate individual spaces for items in that section. For each of the 10
 *    bits/spaces, a 1 indicates that an item of the section's type is stored
 *    there and a 0 indicates that the space is empty.
 *
 *    Example aisle section: 0x651A
 *
 *                MSB                               LSB
 *                  +-----------+-------------------+
 *                  |0 1 1 0 0 1|0 1 0 0 0 1 1 0 1 0|
 *                  +-----------+-------------------+
 *                  | item id   | section spaces    |
 *      bit offset: 16          10                  0
 *
 *      In this example, the item id is 0b011001, and there are currently 4
 *      items stored in the section (at bit offsets 8, 4, 3, and 1) and 6
 *      vacant spaces.
 *
 *  Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include "aisle_manager.h"
#include "store_util.h"

// the number of total bits in a section
#define SECTION_SIZE 16

// The number of bits in a section used for the item spaces
#define NUM_SPACES 10

// The number of bits in a section used for the item id
#define ID_SIZE 6

// The number of sections in an aisle
#define NUM_SECTIONS 4

// Mask for extracting a section from the least significant bits of an aisle.
// (aisle & SECTION_MASK) should preserve a section's worth of bits at the
// lower end of the aisle and set all other bits to 0. This is essentially
// extracting section 0 from the example aisle shown above.
#define SECTION_MASK 0xFFFF

// Mask for extracting the spaces bits from a section.
// (section & SPACES_MASK) should preserve all the spaces bits in a section and
// set all non-spaces bits to 0.
#define SPACES_MASK 0x03FF

// Mask for extracting the ID bits from a section.
// (section & ID_MASK) should preserve all the id bits in a section and set all
// non-id bits to 0.
#define ID_MASK 0xFC00


/* Given a pointer to an aisle and a section index, return the section at the
 * given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_section(unsigned long* aisle, int index) {
  return (*aisle >> (index * SECTION_SIZE)) & SECTION_MASK;
}

/* Given a pointer to an aisle and a section index, return the spaces of the
 * section at the given index of the given aisle. The returned short should
 * have the least 10 significant bits set to the spaces and the 6 most
 * significant bits set to 0.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_spaces(unsigned long* aisle, int index) {
  return (*aisle >> (index * SECTION_SIZE)) & SPACES_MASK;
}

/* Given a pointer to an aisle and a section index, return the id of the
 * section at the given index of the given aisle. The returned short should
 * have the least 6 significant bits set to the id and the 10 most significant
 * bits set to 0.
 *
 * Example: if the section is 0b0110010100011010, return 0b0000000000011001.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_id(unsigned long* aisle, int index) {
  unsigned long section = get_section(aisle, index);
	return (section & ID_MASK) >> NUM_SPACES;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for section, set the section at the given index of
 * the given aisle to the new bit pattern.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_section(unsigned long* aisle, int index, unsigned short new_section) {
  // clear the section first
  unsigned long shift = index * SECTION_SIZE; // shift to the correct section
  unsigned long mask = (unsigned long) SECTION_MASK << shift; // mask for clearing the section
  *aisle &= ~mask;

  // set the new section
  *aisle |= (unsigned long) new_section << shift;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the spaces of the section, set the spaces for the
 * section at the given index of the given aisle to the new bit pattern.
 * 
 * If new_spaces is invalid (if it has 1s outside of its 10 least significant
 * bits), this method should return without modifying anything.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_spaces(unsigned long* aisle, int index, unsigned short new_spaces) {
  // check if new_spaces is valid, e.g., has 1s outside of its 10 least significant bits
  if ((new_spaces >> NUM_SPACES) != 0) {
    return;
  }
  
  unsigned short section = get_section(aisle, index);
	section = (section & ~SPACES_MASK) | new_spaces; // clear the spaces and set new spaces
	set_section(aisle, index, section);
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the id of the section, set the id for the section
 * at the given index of the given aisle to the new bit pattern.
 * 
 * If new_id is invalid (if it has 1s outside of its 6 least significant
 * bits), this method should return without modifying anything.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_id(unsigned long* aisle, int index, unsigned short new_id) {
  // check if new_id is valid (must fit within 6 bits)
	if (new_id >> ID_SIZE) {
    return;
  }
	
	unsigned short section = get_section(aisle, index);
	section = (section & ~ID_MASK) | (new_id << NUM_SPACES); // clear the id and set new id
	set_section(aisle, index, section);
}

/* Given a pointer to an aisle, a section index, and a space index, toggle the
 * item in the given space index of the section at the given section index in
 * the given aisle. Toggling means that if the space was previously empty, it
 * should now be filled with an item, vice-versa.
 *
 * Can assume the section index is a valid index (0-3 inclusive).
 * Can assume the spaces index is a valid index (0-9 inclusive).
 */
void toggle_space(unsigned long* aisle, int index, int space_index) {
  unsigned short section = get_section(aisle, index); // get the section
  unsigned short toggle_mask = (unsigned short)(1 << space_index); // mask for toggling the bit at given space_index

  section ^= toggle_mask; // toggle the bit at given space_index
  set_section(aisle, index, section);
}

/* Given a pointer to an aisle and a section index, return the number of items
 * in the section at the given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short num_items(unsigned long* aisle, int index) {
  unsigned short spaces = get_spaces(aisle, index);
  unsigned short count = 0;

  for (int i = 0; i < NUM_SPACES; i++) {
    if (spaces & 1) { // check if the least significant bit is 1
      count++;
    }
    spaces >>= 1; // right shift to check the next bit
  }

  return count;
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to add, add at most the given number of items to the section at the
 * given index in the given aisle. Items should be added to the least
 * significant spaces possible. If n is larger than or equal to the number of
 * empty spaces in the section, then the section should appear full after the
 * method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void add_items(unsigned long* aisle, int index, int n) {
  unsigned short spaces = get_spaces(aisle, index);

  for (int i = 0; i < NUM_SPACES && n > 0; i++) {
    unsigned short bit_mask = (unsigned short)(1 << i);

    if ((spaces & bit_mask) == 0) { // if the space is empty
        spaces |= bit_mask;         // add an item to the space
        n--;                        // decrement the number of items to add
    }
  }

  set_spaces(aisle, index, spaces);
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to remove, remove at most the given number of items from the section
 * at the given index in the given aisle. Items should be removed from the
 * least significant spaces possible. If n is larger than or equal to the
 * number of items in the section, then the section should appear empty after
 * the method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void remove_items(unsigned long* aisle, int index, int n) {
  unsigned short spaces = get_spaces(aisle, index);

  for (int i = 0; i < NUM_SPACES && n > 0; i++) {
    unsigned short bit_mask = (unsigned short)(1 << i);

    if (spaces & bit_mask) {  // if the space is filled
        spaces ^= bit_mask;   // remove the item from the space
        n--;                  // decrement the number of items to remove
    }
  }

  set_spaces(aisle, index, spaces);
}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the left by the given number of slots.
 *
 * Example: if the spaces are 0b0111100001, then rotating left by 2 results
 *          in the spaces     0b1110000101
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_left(unsigned long* aisle, int index, int n) {
  n %= NUM_SPACES; // find an equivalent rotation, e.g., rotating left by 12 is the same as rotating left by 2

  if (n > 0) { // only rotate if n > 0 to avoid unnecessary work
    unsigned short spaces = get_spaces(aisle, index);
    unsigned short rotated_spaces = (spaces << n) | (spaces >> (NUM_SPACES - n)); // right shift wraps around the bits shifted out on the left
    rotated_spaces &= SPACES_MASK; // ensure only the least 10 significant bits are kept

    set_spaces(aisle, index, rotated_spaces);
  }
}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the right by the given number of slots.
 *
 * Example: if the spaces are 0b1000011110, then rotating right by 2 results
 *          in the spaces     0b1010000111
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_right(unsigned long* aisle, int index, int n) {
  n %= NUM_SPACES; // find equivalent rotation
  if (n > 0) { // only rotate if n > 0 to avoid unnecessary work
    rotate_items_left(aisle, index, NUM_SPACES - n); // rotating right by n is the same as rotating left by (NUM_SPACES - n)
  }
}  