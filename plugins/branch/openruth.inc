<?php

// $Id$

/**
 * @file
 * Ding branch plugin.
 */

$plugin = array(
  'description' => t('Openruth branch plugin'),
  'version' => 1,
);

/**
 */
function openruth_branch_list() {
  $counters = openruth_client()->get_agency_counters();
  if ($counters and !is_string($counters)) {
    return $counters;
  }
  return array();
}
