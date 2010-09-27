<?php

// $Id$

/**
 * @file
 * Ding user plugin.
 */

$plugin = array(
  'description' => t('OpenRuth user provider'),
  // Version compatibility.
  'version' => 1,
);

/**
 * Get information on a user.
 *
 * Returns an array of user data, which as a minimum should contain
 * the keys 'user_id' and 'user_name', which is used to bind to a
 * Drupal user.
 *
 * @param object $account
 *   User object.
 * @param boolean $extended
 *   Optional, whether to return extended information.
 *
 * @return array
 *   An array of user information.
 */
function openruth_user_get_info($account, $extended = FALSE) {
  $creds = _alma_user_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return $creds;
  }
  return alma_client_get_patron_info($creds['user_id'], $creds['password'], $extended);
}

/**
 * Returns whether the account is currently 'logged in' to the user
 * backend.
 *
 * If not, we'll need to authenticate before we can do anything.
 *
 * @param object $account
 *   The user to check.
 * @param boolean $redirect
 *   Whether it's OK to redirect to log the user in.
 *
 * @return boolean
 *   TRUE if the account is authenticated.
 */
function openruth_user_logged_in($account, $redirect = FALSE) {
  $creds = _openruth_get_credentials($account);
  return $creds != DING_PROVIDER_AUTH_REQUIRED;
}
