<?php
// $Id$

/**
 * @file openruth.admin.inc
 * Admin settings for the OpenRuth Drupal module.
 */

/**
 * Main settings form implementation.
 */
function openruth_admin_settings_form($form_state) {
  $form = array();

  $form['openruth_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('OpenRuth base URL'),
    '#description' => t('The base URL for constructing request to the OpenRuth SOAP service, usually something like https://server.example.com:8000/openruth/'),
    '#required' => TRUE,
    '#default_value' => variable_get('openruth_base_url', ''),
  );
  $form['openruth_enable_logging'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable logging'),
    '#default_value' => variable_get('openruth_enable_logging', ''),
    '#description' => t('Logs requests to the OpenRuth webservice. Sensitive information such as CPR number and PIN code is stripped from the requests'),
  );
  
  return system_settings_form($form);
}

