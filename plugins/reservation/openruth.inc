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
        if (isset($order->itemId)) {
          $reservations[$order->orderId]['provider_id'] = $order->itemId;
        }
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
        if (isset($order->itemId)) {
          $reservations[$order->orderId]['provider_id'] = $order->itemId;
        }
      }
    }
  }
  if ($user_status && isset($user_status->illOrders) && isset($user_status->illOrders->illOrder)) {
    foreach ($user_status->illOrders->illOrder as $order) {
      $reservations[$order->orderId] = array(
        'remote_reservation' => TRUE,
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
      );
      if (isset($order->itemId)) {
        $reservations[$order->orderId]['provider_id'] = $order->itemId;
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
    if ($reservation['provider_id'] and $object = ting_get_object_by_local_id($reservation['provider_id'])) {
      $reservation['ting_object'] = $object;
    }
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
  $creds = ding_library_user_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return array('status' => $creds);
  }
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

  $order_res = openruth_client_invoke('order_item', $creds['name'], $provider_id, $optional['expiry'], $optional['pickup_branch']);
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

/**
 *
 */
function openruth_reservation_options($account, $create = TRUE) {
  $form['expiry'] = array(
    '#type' => 'date_popup',
    '#title' => t('Valid to'),
    '#date_format' => variable_get('date_format_date_short', DATE_FORMAT_DATE),
    '#date_year_range' => '-0:+2',
    '#required' => $create,
  );

  if ($create) {
    $interest_period = (isset($account->interest_period) && $account->interest_period > 10) ? $account->interest_period : DING_LIBRARY_USER_DEFAULT_INTEREST_PERIOD;
    $form['expiry']['#default_value'] = date('Y-m-d', $_SERVER['REQUEST_TIME'] + $interest_period * 86400);
  }

  $branches = array();
  if (!$create) {
    $branches = array(
      '' => t("No change"),
    );
  }
  $branches += _openruth_get_agency_counters();

  $form['pickup_branch'] = array(
    '#type' => 'select',
    '#title' => t('New pickup branch'),
    '#options' => $branches,
    '#default_value' => (isset($account->preferred_branch)) ? $account->preferred_branch : NULL, 
    '#required' => $create,
  );

  if ($create) {
    $form['#element_validate'] = array('openruth_element_validate_reservation_options');
  }

  return $form;
}

/**
 *
 */
function openruth_reservation_update($account, $reservation_ids, $options = array()) {
  $res = array();
  $reservations = openruth_reservation_list($account);
  foreach ($reservation_ids as $reservations_id) {
    $expiry = empty($options['expiry']) ?
      $reservations[$reservations_id]['valid_to'] :
      $options['expiry'];
    $pickup_branch = empty($options['pickup_branch']) ?
      $reservations[$reservations_id]['pickup_branch'] :
      $options['pickup_branch'];
    $res[] = openruth_client_invoke('update_order', $reservations_id, $pickup_branch, $expiry);
  }

  // Flush cached user status.
  _openruth_user_status();

  return $res;
}

/**
 *
 */
function openruth_reservation_delete($account, $reservation_ids) {
  $res = array();
  foreach ($reservation_ids as $reservation_id) {
    $res = openruth_client_invoke('cancel_order', $reservation_id);
  }

  // Flush cached user status.
  _openruth_user_status();

  return $res;
}
