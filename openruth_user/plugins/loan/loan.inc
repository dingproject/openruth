<?php

// $Id$

/**
 * @file
 * Ding loan plugin.
 */

$plugin = array(
  'description' => t('OpenRuth loan plugin'),
  'version' => 1,
);

/**
 */
function openruth_user_loan_get_loans($account) {
  return array();
  $creds = _ding_provider_example_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return $creds;
  }
  // As we doesn't really have a backend, we just fake it.
  // Eeeh..
  return array();
}

/**
 */
function openruth_user_loan_renew_loan($account, $loan) {
  $creds = _ding_provider_example_get_credentials($account);
  $reservations = alma_client_get_reservations($creds['user_id'], $creds['password/email/etc']);
  return $reservations;
}
