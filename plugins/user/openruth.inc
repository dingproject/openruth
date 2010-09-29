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

/**
 * Form callback for plugin settings.
 *
 * This is a regular form callback.
 */
function openruth_user_settings_form() {
  $form['openruth_wsdl_url'] = array(
    '#type' => 'textfield',
    '#title' => t('OpenRuth WSDL URL'),
    '#description' => t('The WSDL URL for OpenRuth SOAP service, usually something like http://openruth.addi.dk/0.2/openruth.wsdl'),
    '#required' => TRUE,
    '#default_value' => variable_get('openruth_wsdl_url', ''),
  );

  $form['openruth_agency_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Agency Id'),
    '#default_value' => variable_get('openruth_agency_id', ''),
    '#description' => t('The OpenRuth agency id of the library.'),
  );

  $form['openruth_enable_logging'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable logging'),
    '#default_value' => variable_get('openruth_enable_logging', FALSE),
    '#description' => t('Logs requests to the OpenRuth webservice. Sensitive information such as CPR number and PIN code is stripped from the requests.'),
  );

  $form['openruth_credential_cache_duration'] = array(
    '#type' => 'select',
    '#title' => t('Credential cache duration'),
    '#description' => t('Determines how long the user’s library system credentials will be cached for. For the duration of the cache, the user will be able to access his personal information and interact with the library system without having to provide his credentials again.'),
    '#options' => array(
      300 => t('5 minutes'),
      900 => t('15 minutes'),
      1800 => t('30 minutes'),
      3600 => t('1 hour'),
      10800 => t('3 hours'),
      43200 => t('12 hours'),
      86400 => t('1 day'),
      259200 => t('3 days'),
      604800 => t('7 days'),
    ),
    '#default_value' => variable_get('openruth_credential_cache_duration', 1800),
  );

  return system_settings_form($form);
}
