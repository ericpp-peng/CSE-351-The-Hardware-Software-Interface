/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s):  Po Peng, Kevin Le
 * NetID(s): ericpp, kevin218
 *
 * This is a file for managing a store of various aisles, represented by an
 * array of 64-bit integers. See aisle_manager.c for details on the aisle
 * layout and descriptions of the aisle functions that you may call here.
 *
 * Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include <stddef.h>  // To be able to use NULL
#include "aisle_manager.h"
#include "store_client.h"
#include "store_util.h"

// Number of aisles in the store
#define NUM_AISLES 10

// Number of sections per aisle
#define SECTIONS_PER_AISLE 4

// Number of items in the stockroom (2^6 different id combinations)
#define NUM_ITEMS 64

// The number of bits in a section used for the item spaces
#define NUM_SPACES 10

// Global array of aisles in this store. Each unsigned long in the array
// represents one aisle.
unsigned long aisles[NUM_AISLES];

// Array used to stock items that can be used for later. The index of the array
// corresponds to the item id and the value at an index indicates how many of
// that particular item are in the stockroom.
int stockroom[NUM_ITEMS];


/* Starting from the first aisle, refill as many sections as possible using
 * items from the stockroom. A section can only be filled with items that match
 * the section's item id. Prioritizes and fills sections with lower addresses
 * first. Sections with lower addresses should be fully filled (if possible)
 * before moving onto the next section.
 */
void refill_from_stockroom() {

  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {

      unsigned short item_id = get_id(&aisles[i], j);

      for (int k = 0; k < NUM_ITEMS; k++) {
        if (stockroom[k] > 0) {
          if (k == item_id) {
            unsigned short nums = num_items(&aisles[i], j);
            unsigned short empty_num = NUM_SPACES - nums;
            // stock grater than empty space, fill all empty space
            if (stockroom[k] >= empty_num) {
              add_items(&aisles[i], j, empty_num);
              stockroom[k] -= empty_num;
            }
            else { // stock less than empty space, use all stock
              add_items(&aisles[i], j, stockroom[k]);
              stockroom[k] = 0;
            }
          }
        }
      }
    }
  }
}

/* Remove at most num items from sections with the given item id, starting with
 * sections with lower addresses, and return the total number of items removed.
 * Multiple sections can store items of the same item id. If there are not
 * enough items with the given item id in the aisles, first remove all the
 * items from the aisles possible and then use items in the stockroom of the
 * given item id to finish fulfilling an order. If the stockroom runs out of
 * items, you should remove as many items as possible.
 */
int fulfill_order(unsigned short id, int num) {

  int total_removed = 0;
  int remaining_to_remove = num;

  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short item_id = get_id(&aisles[i], j);
      if (item_id == id) {
        unsigned short num_item_in_section = num_items(&aisles[i], j);
        // enough items in aisle
        if (num_item_in_section >= remaining_to_remove) {
          remove_items(&aisles[i], j, remaining_to_remove);
          total_removed += remaining_to_remove;
          return total_removed;
        }
        else { // not enough items in aisle
          remove_items(&aisles[i], j, num_item_in_section);
          remaining_to_remove -= num_item_in_section;
          total_removed += num_item_in_section;
        }
      }
    }
  }

  if (remaining_to_remove == 0) {
    return total_removed;
  }
  else if (remaining_to_remove > 0) {
    // enough items in stockroom
    if (stockroom[id] >= remaining_to_remove) {
      stockroom[id] -= remaining_to_remove;
      total_removed += remaining_to_remove;
      return total_removed;
    }
    else { // not enough items in stockroom
      total_removed += stockroom[id];
      stockroom[id] = 0;
      return total_removed;
    }
  }

  return -1;
}

/* Return a pointer to the first section in the aisles with the given item id
 * that has no items in it or NULL if no such section exists. Only consider
 * items stored in sections in the aisles (i.e., ignore anything in the
 * stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* empty_section_with_id(unsigned short id) {

  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short item_id = get_id(&aisles[i], j);
      if (item_id == id) {
        unsigned short num_item_in_section = num_items(&aisles[i], j);
        if (num_item_in_section == 0) {
          return (unsigned short*) (&aisles[i]) + j;
        }
      }
    }
  }

  return NULL;
}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* section_with_most_items() {

  int max_items = -1;
  unsigned short* result_section = NULL;

  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short num_item_in_section = num_items(&aisles[i], j);
      if (num_item_in_section > max_items) {
        max_items = num_item_in_section;
        result_section = (unsigned short*) (&aisles[i]) + j;
      }
    }
  }

  return result_section;
}