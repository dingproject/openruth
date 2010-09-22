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

