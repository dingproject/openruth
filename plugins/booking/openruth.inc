<?php

// $Id$

/**
 * @file
 * Ding booking plugin.
 */

$plugin = array(
  'description' => t('Openruth booking plugin'),
  'version' => 1,
);

/**
 */
function openruth_booking_list($account) {
  static $state_mapping = array(
    'deficit' => 'deficit',
    'active' => 'waiting',
    'registered' => 'waiting',
    'partly retained' => 'waiting',
    'back order' => 'waiting',
    'retained' => 'ready',
    'on loan' => 'active',
    'partly returned' => 'active',
    'partly on loan' => 'active',
    'closed' => 'closed',
  );
  $user_status = _openruth_user_status($account);
  if ($user_status == DING_PROVIDER_AUTH_REQUIRED) {
    return $user_status;
  }
  $bookings = array();
  if ($user_status && isset($user_status->bookings)) {
    if (isset($user_status->bookings->booking)) {
      foreach ($user_status->bookings->booking as $booking) {
        $bookings[$booking->bookingId] = array(
          'id' => $booking->bookingId,
          'display_title' => $booking->itemDisplayTitle,
          'status' => $state_mapping[$booking->bookingStatus],
          'start_date' => $booking->bookingStartDate,
          'end_date' => $booking->bookingEndDate,
          'count' => $booking->bookingTotalCount,
          'fetched_count' => $booking->bookingFetchedCount,
          'loaned_count' => $booking->bookingLoanedCount,
          'returned_count' => $booking->bookingReturnedCount,
          'pickup_branch' => $booking->agencyCounter,
        );
      }
    }
  }
  return $bookings;
}

/**
 *
 */
function openruth_booking_options($account) {
  $form['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('From'),
    '#required' => TRUE,
    '#date_format' => DATE_FORMAT_DATE,
    '#date_year_range' => '-0:+2',
    '#element_validate' => array('ding_base_element_validate_future_date'),
  );

  $form['end_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('To'),
    '#required' => TRUE,
    '#date_format' => DATE_FORMAT_DATE,
    '#date_year_range' => '-0:+2',
    '#element_validate' => array('ding_base_element_validate_future_date'),
  );

  $form['count'] = array(
    '#type' => 'textfield',
    '#title' => t('Number to book'),
    '#required' => TRUE,
    '#default_value' => '1',
    '#element_validate' => array('ding_base_element_validate_positive_integer'),
  );

  $form['pickup_branch'] = array(
    '#type' => 'select',
    '#title' => t('Pickup branch'),
    '#required' => TRUE,
    '#options' => _openruth_get_agency_counters(),
  );

  $form['#element_validate'] = array('openruth_element_validate_options');
  return $form;
}

/**
 * Create a booking.
 *
 * @param array $account
 *   The user to book for.
 * @param array $provider_id
 *   Id to book.
 * @return array
 *   with the following keys:
 *   - success:
 *     One of the following constants:
 *     - DING_PROVIDER_AUTH_REQUIRED: Authentication required.
 *     - DING_BOOKING_SUCCESS: Successfully reserved.
 *     - DING_BOOKING_ERROR: General error (use sparingly).
 *     Any other value is treated like DING_BOOKING_ERROR.
 *   - message: (optional) In case of DING_BOOKING_ERROR, an optional error
 *     message which is displayed to the user instead of the standard error
 *     message.
 */
function openruth_booking_create($account, $provider_id, $optional = array()) {
  // We don't check the options as we assume it's directly from our own form.

  $creds = ding_library_user_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return array('status' => $creds);
  }

  $book_res = openruth_client_invoke('book_item', $creds['name'], $provider_id, $optional['count'], $optional['start_date'], $optional['end_date'], $optional['pickup_branch']);
  if (is_string($book_res)) {
    $res = array('status' => DING_BOOKING_ERROR);
    if ($message = openruth_booking_user_error($renew_res)) {
      $res['message'] = $message;
    }
  }
  else {
    $res = array('status' => $book_res ? DING_BOOKING_SUCCESS : DING_BOOKING_ERROR);
  }

  // Flush cached user status.
  _openruth_user_status();
  return $res;
}
