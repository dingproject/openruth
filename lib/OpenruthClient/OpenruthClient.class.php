<?php
// $Id$

class OpenruthClient {

  /**
   * Our SOAP client.
   **/
  private $client;

  /**
   * The OpenRuth wsdl url.
   **/
  private $wsdl_url;

  /**
   * The OpenRuth agency id.
   **/
  private $agency_id;

  /**
   * Whether we're logging requests.
   */
  private $logging = FALSE;

  /**
   * Start time of request, used for logging.
   */
  private $log_timestamp = NULL;

  /**
   * The salt which will be used to scramble sensitive information in logging
   * across all requests for the page load.
   */
  private static $salt;

  /**
   * Constructor
   */
  public function __construct($wsdl_url, $agency_id) {
    $this->wsdl_url = $wsdl_url;
    $this->agency_id = $agency_id;
    $options = array();
    if (variable_get('openruth_enable_logging', FALSE)) {
      $this->logging = TRUE;
      $options['trace'] = TRUE;
    }
    $this->client = new SoapClient($this->wsdl_url, $options);
    self::$salt = rand();
  }

  /**
   * Log start of request, for adding to log entry later.
   */
  private function log_start() {
    if ($this->logging) {
      $this->log_timestamp = microtime(TRUE);
    }
  }

  /**
   * Log last request, if logging is enabled.
   *
   * @param ...
   *   A variable number of arguments, whose values will be redacted.
   */
  private function log() {
    if ($this->logging) {
      if ($this->log_timestamp) {
        $time = round(microtime(TRUE) - $this->log_timestamp, 2);
        $this->log_timestamp = NULL;
      }
      $sensitive = func_get_args();
      // For some reason PHP doesn't have array_flatten, and this is the
      // shortest alternative.
      $replace_values = array();
      foreach (new RecursiveIteratorIterator(new RecursiveArrayIterator($sensitive)) as $value) {
        $replace_values['>' . $value . '<'] = '>' . substr(md5($value . self::$salt), 0, strlen($value)) . '<';
      }
      if ($time) {
        watchdog('openruth', 'Sending request (@seconds sec): @xml',array('@xml' => strtr($this->client->__getLastRequest(), $replace_values), '@seconds' => $time), WATCHDOG_DEBUG);

      } else {
        watchdog('openruth', 'Sending request: @xml',array('@xml' => strtr($this->client->__getLastRequest(), $replace_values)), WATCHDOG_DEBUG);
      }
    }
  }

  /**
   * Perform request to the OpenRuth server
   */
  public function request() {}

  /**
   * Get information about an agency's counters
   */
  public function get_agency_counters() {
    $this->log_start();
    $res = $this->client->agencyCounters(array(
             'agencyId' =>  $this->agency_id,
           ));
    $this->log();
    if (isset($res->agencyError)) {
      return $res->agencyError;
    }
    elseif (isset($res->agencyCounters) && isset($res->agencyCounters->agencyCounterInfo)) {
      $result = array();
      foreach ($res->agencyCounters->agencyCounterInfo as $agencyCounterInfo) {
        $result[$agencyCounterInfo->agencyCounter] = $agencyCounterInfo->agencyCounterName;
      }
      return $result;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Holdings information (agency info, location, availability etc.) about an given item.
   */
  public function get_holdings() {}

  /**
   * Renewing one or more loans
   */
  public function renew_loan($username, $copy_ids) {
    $this->log_start();
    $res = $this->client->renewLoan(array(
             'agencyId' =>  $this->agency_id,
             'userId' => $username,
             'copyId' => $copy_ids,
      ));
    $this->log($username);
    if (isset($res->renewLoanError)) {
      return $res->renewLoanError;
    }
    elseif (isset($res->renewLoan)) {
      $result = array();
      foreach ($res->renewLoan as $renewLoan) {
        $result[$renewLoan->copyId] = isset($renewLoan->renewLoanError) ? $renewLoan->renewLoanError : TRUE;
      }
      return $result;
    }
    else {
      return FALSE;
    }
  }

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
  public function cancel_order($order_id) {
    $this->log_start();
    $res = $this->client->cancelOrder(array(
             'agencyId' =>  $this->agency_id,
             'orderId' => $order_id,
      ));
    $this->log();
    if (isset($res->cancelOrderError)) {
      return $res->cancelOrderError;
    }
    elseif (isset($res->cancelOrderOk)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Changing userinfo (pincode, contact, preferences etc.)
   */
  public function update_userinfo() {}

  /**
   * Performing a user check (whether the user exists in the system and user's various rights and parameters)
   *
   * @return mixed
   *   UserCheck response object, a string error message or FALSE.
   */
  public function user_check($username, $pin_code) {
    $res = $this->client->userCheck(array(
             'agencyId' =>  $this->agency_id,
             'userId' => $username,
             'userPinCode' => $pin_code,
      ));
    $this->log($username, $pin_code);
    if (isset($res->userError)) {
      return $res->userError;
    }
    elseif (isset($res->userCheck)) {
      return $res->userCheck;
    }
    else {
      return FALSE;
    }
  }

  /**
   * User's status, includings loans, orders, ILLs and fines
   */
  public function user_status($username, $pin_code) {
    $this->log_start();
    $res = $this->client->userStatus(array(
             'agencyId' =>  $this->agency_id,
             'userId' => $username,
             'userPinCode' => $pin_code,
      ));
    $this->log($username, $pin_code);
    if (isset($res->userError)) {
      return $res->userError;
    }
    elseif (isset($res->userStatus)) {
      return $res->userStatus;
    }
    else {
      return FALSE;
    }
  }

  /**
   * paying user fines
   */
  public function user_payment() {}

}