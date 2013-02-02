<?php

/**
 * @file
 * This file contains helper functions used throughout this module.
 */

// Constants.
define('TESSERACT_REQUIRED_VERSION', '3.02');

/**
 * Checks if it is possible to create files with tesseract.
 *
 * @return bool
 *   TRUE if it is possible, FALSE otherwise.
 */
function islandora_ocr_can_derive_ocr() {
  $version = islandora_ocr_get_tesseract_version();
  return version_compare($version, TESSERACT_REQUIRED_VERSION) >= 0;
}

/**
 * Gets the versions of the given tesseract executable.
 *
 * @param string $tesseract
 *   The absolute path to $tesseract executable, if not provided the variable
 *   "islandora_ocr_tesseract" will be used instead.
 *
 * @return string
 *   The version of the tesseract executable if successful, FALSE otherwise.
 */
function islandora_ocr_get_tesseract_version($tesseract = NULL) {
  $tesseract = $tesseract ? $tesseract : variable_get('islandora_ocr_tesseract', '/usr/bin/tesseract');
  if (is_executable($tesseract)) {
    $ret = -1;
    $output = array();
    $command = "$tesseract -v 2>&1";
    exec($command, $output, $ret);
    if (!$ret && isset($output[0])) {
      return str_replace(array('tesseract', ' '), '', $output[0]);
    }
  }
  return FALSE;
}

/**
 * Gets tesseracts installed languages.
 *
 * @param string $tesseract
 *   The absolute path to $tesseract executable, if not provided the variable
 *   "islandora_ocr_tesseract" will be used instead.
 *
 * @return array
 *   The installed languages where the value is in a form tesseract recognizes.
 */
function islandora_ocr_get_tesseract_installed_languages($tesseract = NULL) {
  $tesseract = $tesseract ? $tesseract : variable_get('islandora_ocr_tesseract', '/usr/bin/tesseract');
  if (is_executable($tesseract)) {
    $ret = -1;
    $output = array();
    $command = "$tesseract --list-langs 2>&1";
    exec($command, $output, $ret);
    if (!$ret && count($output) > 1) {
      // Remove the descriptive first line, "List of available languages (n):"
      array_shift($output);
      return $output;
    }
  }
  return array();
}

/**
 * Gets the human readable name for the given tesseract language abbreviation.
 *
 * @param string $language
 *   A tesseract recognized language abbreviation.
 *
 * @return string
 *   A translated human readable name for the given abbreviation if found,
 *   otherwise the abbreviation is returned unaltered.
 */
function islandora_ocr_tesseract_language_name($language) {
  $language_names = array(
    'eng' => t('English'),
    'fra' => t('French'),
    'deu-frak' => t('German'),
    'por' => t('Portugese'),
    'spa' => t('Spanish'),
    'hin' => t('Hindi'),
    'jpn' => t('Japanese'),
    'ita' => t('Italian')
  );
  return isset($language_names[$language]) ? $language_names[$language] : $language;
}

/**
 *   The supported languages where the key is the value tesseract recognizes and
 *   the value is a human readable version of the language.
      $installed_languages = array_combined($output, $output);
      $human_readable_names = array(
        'eng' => t('English'),
        'fra' => t('French'),
        'deu-frak' => t('German'),
        'por' => t('Portugese'),
        'spa' => t('Spanish'),
        'hin' => t('Hindi'),
        'jpn' => t('Japanese'),
        'ita' => t('Italian')
      );
      return array_intersect_keys($human_readable_names, $installed_languages);

 */

/**
 * Gets only the enabled languages as defined by the user in the admin form.
 *
 * @return array
 *   The enabled languages where the key is the value tesseract recognizes and
 *   the value is a human readable version of the language.
 */
function islandora_ocr_get_enabled_tesseract_languages() {
  $tesseract_languages = islandora_ocr_get_tesseract_languages();
  $enabled_languages = array_filter(variable_get('islandora_ocr_tesseract_enabled_languages', $tesseract_languages));
  return array_intersect_key($tesseract_languages, $enabled_languages);
}

/**
 * Converts the given image to a plain text OCR file, using the given options.
 *
 * @param string $image_file
 *   The absolute path to the image file.
 * @param array $options
 *   The options to be passed to tesseract.
 *
 * @return string
 *   The absolute path to the generated OCR if successful, FALSE otherwise.
 */
function islandora_ocr_convert_to_ocr($image_file, array $options) {
  $out_file = "$image_file.txt";
  $executable = variable_get('islandora_ocr_tesseract', '/usr/bin/tesseract');
  $output = array(); $ret = 0;
  $command = "{$executable} {$image_file} {$image_file} -l {$options['language']} 2>&1";
  exec($command, $output, $ret);
  if ($ret != '0') {
    $message = 'tesseract failed to create ocr<br/>Error: @ret<br/>Command: @command<br/>Output: !output';
    $variables = array(
      '@ret' => $ret,
      '@command' => $command,
      '!output' => implode('<br/>', $output),
    );
    watchdog('islandora_ocr', $message, $variables, WATCHDOG_ERROR);
    return FALSE;
  }
  return $out_file;
}

/**
 * Converts the given image to a html HOCR file, using the given options.
 *
 * @param string $image_file
 *   The absolute path to the image file.
 * @param array $options
 *   The options to be passed to tesseract.
 *
 * @return string
 *   The absolute path to the generated HOCR if successful, FALSE otherwise.
 */
function islandora_ocr_convert_to_hocr($image_file, array $options) {
  $out_file = "$image_file.html";
  $executable = variable_get('islandora_ocr_tesseract', '/usr/bin/tesseract');
  $output = array(); $ret = 0;
  $command = "{$executable} {$image_file} {$image_file} -l {$options['language']} hocr 2>&1";
  exec($command, $output, $ret);
  if ($ret != '0' || !file_exists($out_file)) {
    $message = 'tesseract failed to create hocr<br/>Error: @ret<br/>Command: @command<br/>Output: !output';
    $variables = array(
      '@ret' => $ret,
      '@command' => $command,
      '!output' => implode('<br/>', $output),
    );
    watchdog('islandora_ocr', $message, $variables, WATCHDOG_ERROR);
    return FALSE;
  }
  if (!islandora_ocr_hocr_valid($out_file)) {
    $message = 'tesseract failed to create valid hocr make sure to check your running a supported version of tesseract 3.2 or later.';
    $message .= '<br/>Error: @ret<br/>Command: @command<br/>Output: !output';
    $variables = array(
      '@ret' => $ret,
      '@command' => $command,
      '!output' => implode('<br/>', $output),
    );
    watchdog('islandora_ocr', $message, $variables, WATCHDOG_ERROR);
    return FALSE;
  }
  return $out_file;
}

/**
 * Checks if the given HOCR file is valid.
 *
 * At the moment this only checks if the version is supported but may do more
 * in the future.
 *
 * @param string $file
 *   The absolute path to the HOCR file.
 *
 * @return bool
 *   TRUE if the HOCR file is valid, FALSE otherwise.
 */
function islandora_ocr_hocr_valid($file) {
  $version = islandora_ocr_hocr_version($file);
  return version_compare($version, TESSERACT_REQUIRED_VERSION) >= 0;
}

/**
 * Gets the version of the give HOCR file.
 *
 * Parses the meta tags in the given file to determine the version. Older
 * versions of tesseract don't include the version information.
 *
 * @param string $file
 *   The absolute path to the HOCR file.
 *
 * @return string
 *   The version if successful, FALSE otherwise.
 */
function islandora_ocr_hocr_version($file) {
  @$doc = simplexml_load_file($file);
  $doc->registerXPathNamespace('ns', 'http://www.w3.org/1999/xhtml');
  $version = (string) array_pop($doc->xpath('/ns:html/ns:head/ns:meta[@name="ocr-system"]/@content'));
  $version = str_replace(array('tesseract', ' '), '', $version);
  return empty($version) ? FALSE : $version;
}
