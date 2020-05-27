<?php

$img = new Imagick();
$img->readImage($argv[1]);

echo "const uint16_t PROGMEM font[] = {\n";

for ($x = 0; $x < 950; $x++) {
	echo "\t0b";
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

echo "};\n";
