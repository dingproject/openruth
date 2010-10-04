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
function openruth_loan_get_loans($account) {
  $creds = _openruth_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return $creds;
  }
  $loans = array();
  $user_status = _openruth_user_status($account);
  if ($user_status && isset($user_status->loans) && isset($user_status->loans->loan)) {
    foreach ($user_status->loans->loan as $loan) {
      $loans[$loan->copyId] = array(
        'id' => $loan->copyId,
        'title' => $loan->itemTitle,
        'authors' => $loan->itemAuthor,
        'display_title' => $loan->itemDisplayTitle,
        'loan_date' => $loan->loanDate,
        'due_date' => $loan->loanReturnDate,
        'is_renewable' => ($loan->loanRenewable == 'renewable'),
      );
    }
  }

  return $loans;
}

/**
 * Loan details callback.
 */
function openruth_loan_details($loan) {
  return $loan;
  $cache = &ctools_static(__FUNCTION__, array());
  if (!isset($cache[$loan['id']])) {
    if ($loan['provider_id'] and $object = ting_get_object_by_local_id($loan['provider_id'])) {
      $loan['ting_object'] = $object;
    }
    $cache[$loan['id']] = $loan;
  }
  return $cache[$loan['id']];
}

/**
 */
function openruth_loan_renew_loans($account, $loan_ids) {
  $creds = _openruth_get_credentials($account);
  if ($creds == DING_PROVIDER_AUTH_REQUIRED) {
    return $creds;
  }

  $res = array();
  $renew_res = openruth_client()->renew_loan($creds['user_id'], $loan_ids);
  if (is_string($renew_res)) {
    $res = openruth_renew_error($renew_res);
  }
  else {
    foreach ($renew_res as $copy_id => $status) {
      if ($status !== TRUE) {
        $res[$copy_id] = openruth_renew_error($status);
      }
      else {
        $res[$copy_id] = $status;
      }
    }
  }

  // Flush cached user status.
  _openruth_user_status();

  return $res;
}

