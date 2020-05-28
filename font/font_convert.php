<?php

// Convert monochrome image (height=16) to uint16_t array

$img = new Imagick();
$img->readImage($argv[1]);

$width = $img->getImageWidth();

for ($x = 0; $x < $width; $x++) {
	echo "0b";
	for ($y = 15; $y >= 0; $y--) {
		$pixel = $img->getImagePixelColor($x, $y);
		//var_dump($pixel->getColor());
		if ($pixel->getColor() == ['r' => 0, 'g' => 0, 'b' => 0, 'a' => 1]) {
			echo '1';
		} else {
			echo '0';
		}
	}
	echo ",\n";
}
