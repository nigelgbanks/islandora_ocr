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
function islandora_book_admin_settings_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_book', 'includes/utilities');
  $get_default_value = function($name, $default) use(&$form_state) {
    return isset($form_state['values'][$name]) ? $form_state['values'][$name] : variable_get($name, $default);
  };
  $tesseract = $get_default_value('islandora_ocr_tesseract', '/usr/bin/tesseract');
  $tesseract_data_directory = $get_default_value('islandora_ocr_tesseract', '/usr/local/share/tessdata');
  $version = islandora_ocr_get_tesseract_version($tesseract);
  $installed_languages = islandora_ocr_get_tesseract_installed_languages();
  $enabled_languages = $get_default_value('islandora_ocr_tesseract_enabled_languages', drupal_map_assoc(array_keys(islandora_ocr_get_tesseract_enabled_languages())));
  return array(
    'ocr_derivative_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('OCR Settings'),
      'islandora_book_tesseract' => array(
        '#type' => 'textfield',
        '#title' => t('Tesseract'),
        '#description' => t('Tesseract is used to generate the OCR datastreams.<br/>!msg', array(
                          '!msg' => islandora_book_admin_settings_form_executable_available_message($tesseract, $version, TESSERACT_REQUIRED_VERSION))),
        '#default_value' => $tesseract,
        '#prefix' => '<div id="tesseract-wrapper">',
        '#suffix' => '</div>',
        '#ajax' => array(
          'callback' => 'islandora_book_admin_settings_form_tesseract_ajax_callback',
          'wrapper' => 'tesseract-wrapper',
          'effect' => 'fade',
          'event' => 'change',
        ),
      ),
      'islandora_book_gimp' => array(
        '#type' => 'textfield',
        '#title' => t('Gimp'),
        '#description' => t('Gimp is used to preprocess the source image before generating the OCR datastreams.<br/>!msg',
                        array('!msg' => islandora_book_admin_settings_form_executable_available_message($gimp))),
        '#default_value' => $gimp,
        '#prefix' => '<div id="gimp-wrapper">',
        '#suffix' => '</div>',
        '#ajax' => array(
          'callback' => 'islandora_book_admin_settings_form_gimp_ajax_callback',
          'wrapper' => 'gimp-wrapper',
          'effect' => 'fade',
          'event' => 'change',
        ),
      ),
      'islandora_book_tesseract_enabled_languages' => array(
        '#type' => 'checkboxes',
        '#title' => t('Languages available for OCR'),
        '#options' => islandora_book_get_tesseract_languages(),
        '#description' => t('Select from the language packs available on your
                             processing server.<br/> These are normally found in
                             <strong>/usr/local/share/tessdata/</strong><br/>
                             Check with your systems admininistrator if you are
                             unsure of availablilty'),
        '#default_value' => $languages,
      ),
  );
}

/**
 * Gets a message to display if the given executable program is available.
 *
 * @param string $path
 *   The path to an executable to check for availability.
 * @param string $version
 *   The version of exectuable.
 * @param string $required_version
 *   The required version of exectuable.
 *
 * @return string
 *   A message in html detailing if the given executable is accessible.
 */
function islandora_ocr_admin_settings_form_executable_available_message($path, $version = NULL, $required_version = NULL) {
  $available = is_executable($path);
  if ($available) {
    $image = theme_image(array('path' => 'misc/watchdog-ok.png', 'attributes' => array()));
    $message = t('Executable found at @path', array('@path' => $path));
    if ($version) {
      $message .= t('<br/>Version: @version', array('@version' => $version));
    }
    if ($required_version) {
      $message .= t('<br/>Required Version: @version', array('@version' => $required_version));
      if (version_compare($version, $required_version) < 0) {
        $image = theme_image(array('path' => 'misc/watchdog-error.png', 'attributes' => array()));
      }
    }
  }
  else {
    $image = theme_image(array('path' => 'misc/watchdog-error.png', 'attributes' => array()));
    $message = t('Unable to locate executable at @path', array('@path' => $path));
  }
  return $image . $message;
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
function islandora_ocr_admin_settings_form_tesseract_ajax_callback(array $form, array $form_state) {
  return $form['islandora_ocr_tesseract'];
}
