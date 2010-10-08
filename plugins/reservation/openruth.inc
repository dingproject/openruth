<?php

// $Id$

/**
 * @file
 * Ding reservation plugin.
 */

$plugin = array(
  'description' => t('Openruth reservation plugin'),
  'version' => 1,
);

/**
 */
function openruth_reservation_list($account) {
  $user_status = _openruth_user_status($account);
  if ($user_status == DING_PROVIDER_AUTH_REQUIRED) {
    return $user_status;
  }
  $reservations = array();
  if ($user_status && isset($user_status->orders)) {
    if (isset($user_status->orders->orderReady)) {
      foreach ($user_status->orders->orderReady as $order) {
        $reservations[$order->orderId] = array(
          'id' => $order->orderId,
          'title' => $order->itemTitle,
          'authors' => $order->itemAuthor,
          'display_title' => $order->itemDisplayTitle,
          'status' => 'fetchable',
          'pickup_number' => $order->orderPickUpId,
          'pickup_expire_date' => $order->orderPickUpDate,
          'pickup_branch' => $order->agencyCounter,
          'create_date' => $order->orderDate,
          // Leave this out?
          'valid_from' => $order->orderDate,
          'valid_to' => $order->orderLastInterestDate,
        );
      }
    }
    if (isset($user_status->orders->orderNotReady)) {
      foreach ($user_status->orders->orderNotReady as $order) {
        $reservations[$order->orderId] = array(
          'id' => $order->orderId,
          'title' => $order->itemTitle,
          'authors' => $order->itemAuthor,
          'display_title' => $order->itemDisplayTitle,
          'status' => 'active',
          'pickup_branch' => $order->agencyCounter,
          'create_date' => $order->orderDate,
          // Leave this out?
          'valid_from' => $order->orderDate,
          'valid_to' => $order->orderLastInterestDate,
          'queue_number' => $order->orderQueuePosition,
        );
      }
    }
  }
  return $reservations;
}

/**
 * Add details to a reservation for detailed display.
 *
 * The 'list' callback should only return minimal data, and leave out
 * data that would requiring querying additional services, for
 * speed. 'details' will be called when more data is needed, and should
 * populate extra data for display.
 *
 * Should implement internal per-request caching as it might be called
 * multiple times for the same reservation, preferably using ctools_static.
 *
 * @param array $reservations
 *   The reservation to add details to.
 * @return
 *   The reservation with details added.
 */
function openruth_reservation_details($reservation) {
  $cache = &ctools_static(__FUNCTION__, array());
  if (!isset($cache[$reservation['id']])) {
    // Eh, anything we need?
    $cache[$reservation['id']] = $reservation;
  }
  return $cache[$reservation['id']];
}


/**
 * Create a reservation.
 *
 * Should use any available user defaults for interest period or
 * pickup branch.
 *
 * @param array $account
 *   The user to reserve for.
 * @param array $provider_id
 *   Id to reserve.
 * @return array
 *   with the following keys:
 *   - success:
 *     One of the following constants:
 *     - DING_PROVIDER_AUTH_REQUIRED: Authentication required.
 *     - DING_RESERVATION_SUCCESS: Successfully reserved.
 *     - DING_RESERVATION_ERROR_NO_DEFAULT_BRANCH: User haven't set a default
 *       branch.
 *     - DING_RESERVATION_ERROR_RESERVATION_EXISTS: Already reserved.
 *     - DING_RESERVATION_ERROR_NOT_AVAILABLE: Not available for reservation.
 *     - DING_RESERVATION_ERROR_NOT_ALLOWED: Reservation is not allowed for
 *       user.
 *     - DING_RESERVATION_ERROR: General error (use sparingly).
 *     Any other value is treated like DING_RESERVATION_ERROR.
 *   - branch: (optional) In case of DING_RESERVATION_SUCCESS, the branch
 *     the material is reserved to.
 */
function openruth_reservation_create($account, $provider_id, $optional = array()) {
  if (!$optional['pickup_branch']) {
    if ($account->preferred_branch) {
      $optional['pickup_branch'] = $account->preferred_branch;
    }
    else {
      return array('status' => DING_RESERVATION_ERROR_NO_DEFAULT_BRANCH);
    }
  }

  if (!$optional['expiry']) {
    $interest_period = $account->interest_period ? $account->interest_period : DING_RESERVATION_DEFAULT_INTEREST_PERIOD;
    $optional['expiry'] = date('Y-m-d', $_SERVER['REQUEST_TIME'] + $interest_period * 86400);
  }

  $creds = ding_library_user_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return array('status' => $creds);
  }
  $order_res = openruth_client()->order_item($creds['name'], $provider_id, $optional['expiry'], $optional['pickup_branch']);
  if (is_string($order_res)) {
    $res = array('status' => openruth_reservation_error_code($renew_res));
  }
  else {
    if (!isset($order_res[$provider_id])) {
      $res = array('status' => DING_RESERVATION_ERROR);
    }
    else {
      $res = array('status' => openruth_reservation_error_code($order_res[$provider_id]), 'branch' => $optional['pickup_branch']);
    }
  }

  // Flush cached user status.
  _openruth_user_status();

  return $res;
}

/**
 * Check if a reservation exists.
 *
 * @param array $account
 *   The user to check for.
 * @param array $provider_id
 *   Id to reserve.
 * @return boolean
 *   TRUE or FALSE.
 */
function openruth_reservation_exists($account, $provider_id) {
  // TODO: implement
  return FALSE;
}
