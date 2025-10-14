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

// Initial value for max_items in section_with_most_items()
#define INITIAL_MAX_ITEMS -1

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
  for (int i = 0; i < NUM_AISLES; i++) { // for each aisle
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) { // for each section

      unsigned short item_id = get_id(&aisles[i], j);
      int available_stock = stockroom[item_id];

      if (available_stock == 0) { // no stock available
        continue;
      }

      int current_items = num_items(&aisles[i], j);
      int empty_slots = NUM_SPACES - current_items;

      if (empty_slots == 0) { // section already full
        continue;
      }
      
      // add either all available stock or fill all empty slots
      int items_to_add = (available_stock < empty_slots) ? available_stock : empty_slots;

      // fill section and update stockroom
      add_items(&aisles[i], j, items_to_add);
      stockroom[item_id] -= items_to_add;
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
  int remaining = num;

  // step 1: remove items from aisles first
  for (int i = 0; i < NUM_AISLES && remaining > 0; i++) { // for each aisle, also check if order is fulfilled
    for (int j = 0; j < SECTIONS_PER_AISLE && remaining > 0; j++) { // for each aisle, also check if order is fulfilled
      unsigned short item_id = get_id(&aisles[i], j);

      if (item_id != id) { // not the item we want
        continue;
      }

      int item_in_section = num_items(&aisles[i], j);
      if (item_in_section == 0) { // no items in this section
        continue;
      }
      
      // remove either all items in section or remaining items needed
      int items_to_remove = (item_in_section < remaining) ? item_in_section : remaining;
      remove_items(&aisles[i], j, items_to_remove);
      total_removed += items_to_remove; // update total removed
      remaining -= items_to_remove; // update remaining items needed
    }
  }

  // early return if order is already fulfilled
  if (remaining == 0) {
    return total_removed;
  }

  // step 2: use stockroom if needed
  int available_stock = stockroom[id];
  int to_remove = (remaining < available_stock) ? remaining : available_stock;

  stockroom[id] -= to_remove; // update stockroom
  total_removed += to_remove; // update total removed

  return total_removed;
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
      
      if (item_id != id) { // not the item we want
        continue;
      }

      unsigned short num_item_in_section = num_items(&aisles[i], j);
      if (num_item_in_section == 0) {
        return ((unsigned short*) &aisles[i]) + j; // return pointer to section
      }
    }
  }

  return NULL; // no such section exists
}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* section_with_most_items() {

  int max_items = INITIAL_MAX_ITEMS; //ensure first section will be larger
  unsigned short* result_section = NULL;

  for (int i = 0; i < NUM_AISLES; i++) {
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short items_in_section = num_items(&aisles[i], j);

      // update max if needed
      if (items_in_section > max_items) { //strictly greater to ensure lowest address in tie
        max_items = items_in_section;
        result_section = ((unsigned short*)&aisles[i]) + j;
      }
    }
  }

  return result_section;
}