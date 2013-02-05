<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ocr_admin_settings_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_ocr', 'includes/utilities');
  $get_default_value = function($name, $default) use(&$form_state) {
    return isset($form_state['values'][$name]) ? $form_state['values'][$name] : variable_get($name, $default);
  };
  $tesseract = $get_default_value('islandora_ocr_tesseract', '/usr/bin/tesseract');
  $version = islandora_ocr_get_tesseract_version($tesseract);
  $installed_languages = islandora_ocr_get_tesseract_installed_languages($tesseract);
  $enabled_languages = $get_default_value('islandora_ocr_tesseract_enabled_languages', drupal_map_assoc($installed_languages));
  return system_settings_form(array(
      '#prefix' => '<div id="tesseract-wrapper">',
      '#suffix' => '</div>',
      'islandora_ocr_tesseract' => array(
        '#type' => 'textfield',
        '#title' => t('Tesseract'),
        '#description' => t('Tesseract is used to generate the OCR and coordinate data.<br/>!msg', array(
                          '!msg' => islandora_executable_available_message($tesseract, $version, TESSERACT_REQUIRED_VERSION))),
        '#default_value' => $tesseract,
        '#ajax' => array(
          'callback' => 'islandora_ocr_admin_settings_form_ajax_callback',
          'wrapper' => 'tesseract-wrapper',
          'effect' => 'fade',
          'event' => 'change',
        ),
      ),
      'islandora_ocr_tesseract_enabled_languages' => array(
        '#type' => 'checkboxes',
        '#title' => t('Languages available for OCR'),
        '#options' => drupal_map_assoc($installed_languages, 'islandora_ocr_tesseract_language_name'),
        '#description' => t('Select from the language packs available on your
                             processing server.<br/> These are normally found in
                             <strong>/usr/local/share/tessdata/</strong><br/>
                             Check with your systems admininistrator if you are
                             unsure of availablilty'),
        '#default_value' => $enabled_languages,
      )));
}

/**
 * Ajax callback for the Tesseract textfield.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The element to render as part the ajax callback.
 */
function islandora_ocr_admin_settings_form_ajax_callback(array $form, array $form_state) {
  return $form;
}
