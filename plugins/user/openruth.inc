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
  $creds = ding_library_user_get_credentials($account);
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

  return system_settings_form($form);
}

/**
 * Form callback for user library settings.
 *
 * This is a regular form callback.
 */
function openruth_user_library_settings_form($form_state, $account) {
  $form = array();

  $form['account'] = array(
    '#type' => 'value',
    '#value' => $account,
  );

  // We don't have anything, but we know that the reservation plugin have.
  return $form;
}

/**
 *
 */
function openruth_user_authenticate($name, $pass) {
  $return = array(
    'result' => FALSE,
  );
  $res = openruth_client_invoke('user_check', $name, $pass);

  if (is_string($res)) {
    watchdog('openruth', 'Error in user_check: @error', array('@error' => $res), WATCHDOG_ERROR);
    return $return;
  }
  elseif (!$res->userFound || !$res->userPinCodeOk) {
    // Not found.
    return $return;
  }

  if ($res->userBlocked) {
    // Tell the user...
  }

  $return['result'] = TRUE;
  $return['creds'] = array(
    'name' => $name,
    'pass' => $pass,
  );

  // Flush the user status cache, so we'll get the newest information.
  _openruth_user_status();

  return $return;
}
