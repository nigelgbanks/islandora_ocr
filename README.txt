CONTENTS OF THIS FILE
---------------------

 * summary
 * requirements
 * installation
 * configuration
 * faq


SUMMARY
-------

Islandora OCR

This module acts a Toolkit for generating OCR and word coordinate information.
At the moment it relies exclusively on Tesseract to generate this information.

REQUIREMENTS
------------

Tesseract:
 http://code.google.com/p/tesseract-ocr/

ImageMagic: (Optional, Required for OCR preprocessing)
 http://www.imagemagick.org/

INSTALLATION
------------

Tesseract:

Tesseract is an OCR engine that was developed at HP Labs between 1985 and 1995
it is currently being developed at Google. Recognized as one of the most
accurate open source OCR engines available, Tesseract will read binary, grey, or
colour images and output text.

A TIFF reader that will read uncompressed TIFF images is also included.
Islandora Book Solution Pack currently uses Tesseract version 3.2, which can be
obtained from the project home page. Lower versions are not supported.

 http://code.google.com/p/tesseract-ocr/

Installation will differ depending on your operating system; please see the
README Wiki for detailed instructions.

 http://code.google.com/p/tesseract-ocr/wiki/ReadMe

CONFIGURATION
-------------

Tesseract:

Tesseract provides many languages which can be downloaded from here:

 http://code.google.com/p/tesseract-ocr/downloads/list

To install just unzip them in your tessdata directory, typically located:

 /usr/local/share/tessdata

If you want to add your own langauges or train your Tesseract for your
specific needs please review the documentation here:

 http://code.google.com/p/tesseract-ocr/wiki/TrainingTesseract3

F.A.Q.
------

If you encounter any problems try the Tesseract FAQ.

http://code.google.com/p/tesseract-ocr/wiki/FAQ
