<?php

// $Id$

/**
 * @file
 * Ding debt plugin.
 */

$plugin = array(
  'description' => t('Openruth debt plugin'),
  'version' => 1,
);

/**
 *
 */
function openruth_debt_settings_form() {
  $form['description'] = array(
    '#type' => 'markup',
    '#value' => t("Ding Dibs Integration module is required for payments."),
  );

  $form['status'] = array(
    '#type' => 'item',
    '#title' => t('Payments'),
    '#value' => function_exists('ding_dibs_payment_prepare') ? t('Enabled') : t('Disabled'),
  );

  return $form;
}

/**
 */
function openruth_debt_list($account) {
  $user_status = _openruth_user_status($account);
  if ($user_status == DING_PROVIDER_AUTH_REQUIRED) {
    return $user_status;
  }

  if ($user_status && $user_status->userInfo->userFeesTotal) {
    $us_total = str_replace(',', '.', $user_status->userInfo->userFeesTotal);
    // Shortcut.
    if ($us_total <= 0) {
      return array();
    }
  }
  $debts = array();
  $id = 1;
  $total = 0;
  if ($user_status && isset($user_status->fees)) {
    if (isset($user_status->fees->fee)) {
      foreach ($user_status->fees->fee as $fee) {
        // Those with invoice number isn't payable by Dibs, so don't show
        // them.
        if (!$fee->feeInvoiceNumber) {
          $debts[$id] = array(
            'id' => $id++, // I know...
            'display_title' => $fee->itemDisplayTitle,
            'payable' => FALSE,
            'date' => $fee->feeDate,
            'amount' => $fee->feeAmount - $fee->feeAmountPaid,
          );
          $total += $fee->feeAmount - $fee->feeAmountPaid;
        }
      }
    }
  }
  if ($total != $us_total) {
    watchdog('openruth', "Debt total (@total) doesn't match userFeesTotal (@us_total)", array('@total' => $total, '@us_total' => $us_total), WATCHDOG_ALERT);
    return array();
  }

  return $debts;
}
