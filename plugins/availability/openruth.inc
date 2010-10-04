<?php

// $Id$

/**
 * @file
 * Ding availability plugin.
 */

$plugin = array(
  'description' => t('OpenRuth availability plugin'),
  'version' => 1,
);

/**
 */
function openruth_availability_items($provider_ids) {
  $holdings = openruth_client()->get_holdings($provider_ids);
  $res = array();
  foreach ($holdings as $id => $holding) {
    $res[$id] = $holding->available;
  }
  return $res;
}

/**
 */
function openruth_availability_details($provider_ids) {
  $holdings = openruth_client()->get_holdings($provider_ids);
  return $holdings;
}
