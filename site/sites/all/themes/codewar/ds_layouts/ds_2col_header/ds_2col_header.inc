<?php
 // Fuction name is ds_LAYOUT_KEY
  function ds_ds_2col_header() {
    return array(
        'label' => t('Two column w/ header stacked'),
        'path' => drupal_get_path('theme', 'e5_zen') . '/ds_layouts/ds_2col_header',
        'regions' => array(
        'header' => t('Header'),
        'left' => t('Left'),
        'right' => t('Right'),
        'footer' => t('Footer'),
    ),
        'css' => FALSE,
    );
  }