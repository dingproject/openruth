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
  $creds = ding_library_user_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return $creds;
  }
  $reservations = array();
  $user_status = _openruth_user_status($creds);
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
 * @return boolean
 *   Success or not.
 */
function openruth_reservation_create($account, $provider_id) {
  // TODO: implement
  return FALSE;
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
