<?php
// $Id$

class OpenruthClient {

  /**
   * @var OpenruthClientBaseURL
   * The OpenRuth base server url
   **/
  private $base_url;

  /**
   * Constructor
   */
  public function __contruct($base_url) {
    $this->base_url = $base_url;
  }
  
  /**
   * Perform request to the OpenRuth server
   */
  public function request() {}
  
  /**
   * Get information about an agency's counters
   */
  public function get_agency_counters() {}
  
  /**
   * Holdings information (agency info, location, availability etc.) about an given item.
   */
  public function get_holdings() {}
  
  /**
   * Renewing one or more loans
   */
  public function renew_loan() {}
  
  /**
   * Booking an item
   */
  public function book_item() {}

  /**
   * Making a reservation in the local system
   */
  public function order_item() {}
  
  /**
   * Get information about number of copies in a booking available at various times
   */
 public function booking_info() {}
 
  /**
   * Updating details about a booking
   */
  public function update_booking() {}
  
  /**
   * Cancelling a booking of an item
   */
  public function cancel_booking() {}

  /**
   * Updating details about a reservation in the local system
   */
 public function update_order() {}

  /**
   * Deleting a reservation
   */
  public function cancel_order() {}

 /**
  * Changing userinfo (pincode, contact, preferences etc.)
  */
 public function update_userinfo() {}
 
  /**
   * Performing a user check (whether the user exists in the system and user's various rights and parameters)
   */
  public function user_check() {}
  
  /**
   * User's status, includings loans, orders, ILLs and fines
   */
  public function user_status() {}
  
  /**
   * paying user fines
   */
  public function user_payment() {}
  
  
}