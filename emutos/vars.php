<?php
error_reporting(E_ALL & ~E_WARNING);
error_reporting(E_ALL);
ini_set("display_errors", 1);
ini_set("track_errors", 1);

$github = 'https://github.com/emutos/emutos';
$github_releases = $github . '/archive/';

if (count($_GET) > 0)
{
	/* if any config value is set in command line,
	   reset all but version & country in session settings.
	   Needed to get a reliable state when providing a ling
	   */
	foreach ($_SESSION['settings'] as $key => $value)
	{
		if ($key != 'emutosversion' && $key != 'romversion' && $key != 'country')
			unset($_SESSION['settings'][$key]);
	}
}

if (file_exists("versions.php"))
{
	/*
	 * versions.php is recreated if someone does a snapshot build
	 */
	include("versions.php");
} else
{
	$versions = array(
		'snapshot' => array('name' => 'Current snapshot', 'archive' => 'master'),
		'1.1.1'    => array('name' => 'Release 1.1.1',    'archive' => 'VERSION_1_1_1'),
		'1.1'      => array('name' => 'Release 1.1',      'archive' => 'VERSION_1_1'),
		'1.0.1'    => array('name' => 'Release 1.0.1',    'archive' => 'VERSION_1_0_1'),
		'1.0.0'    => array('name' => 'Release 1.0',      'archive' => 'VERSION_1_0'),
		'0.9.12'   => array('name' => 'Release 0.9.12',   'archive' => 'VERSION_0_9_12'),
		'0.9.11'   => array('name' => 'Release 0.9.11',   'archive' => 'VERSION_0_9_11'),
		'0.9.10'   => array('name' => 'Release 0.9.10',   'archive' => 'VERSION_0_9_10'),
		'0.9.9.1'  => array('name' => 'Release 0.9.9.1',  'archive' => 'VERSION_0_9_9_1'),
		'0.9.8'    => array('name' => 'Release 0.9.8',    'archive' => 'VERSION_0_9_8'),
		'0.9.7'    => array('name' => 'Release 0.9.7',    'archive' => 'VERSION_0_9_7'),
		'0.9.6'    => array('name' => 'Release 0.9.6',    'archive' => 'VERSION_0_9_6'),
		'0.9.5'    => array('name' => 'Release 0.9.5',    'archive' => 'VERSION_0_9_5'),
		'0.9.4'    => array('name' => 'Release 0.9.4',    'archive' => 'VERSION_0_9_4'),
		'0.9.3'    => array('name' => 'Release 0.9.3',    'archive' => 'VERSION_0_9_3'),
		'0.9.2'    => array('name' => 'Release 0.9.2',    'archive' => 'VERSION_0_9_2'),
		'0.9.1'    => array('name' => 'Release 0.9.1',    'archive' => 'VERSION_0_9_1'),
		'0.9.0'    => array('name' => 'Release 0.9.0',    'archive' => 'VERSION_0_9_0'),
	);
}

$countries = array(
	'us' => array('name' => 'English (US)'),
	'de' => array('name' => 'German'),
	'sg' => array('name' => 'Swiss (German)'),
	'fr' => array('name' => 'French'),
	'uk' => array('name' => 'English (UK)'),
	'es' => array('name' => 'Spanish'),
	'nl' => array('name' => 'Dutch'),
	'it' => array('name' => 'Italian'),
	'fi' => array('name' => 'Finnish'),
	'pl' => array('name' => 'Polish'),
	'no' => array('name' => 'Norwegian'),
	'se' => array('name' => 'Swedish'),
	'pl' => array('name' => 'Polish'),
	'cz' => array('name' => 'Czech'),
	'gr' => array('name' => 'Greek'),
	'ru' => array('name' => 'Russian'),
	'hu' => array('name' => 'Hungarian'),
	'tr' => array('name' => 'Turkish'),
	'ca' => array('name' => 'Catalan'),
);

$targets = array(
	'@192'             => array('filename' => 'etos192@UNIQUE@.img',     'zip' => 'etos-192@UNIQUE@',  'unique' => 1, 'name' => '192k ROM image'),
	'@256'             => array('filename' => 'etos256@UNIQUE@.img',     'zip' => 'etos-256@UNIQUE@',  'unique' => 1, 'name' => '256k ROM image'),
	'@512'             => array('filename' => 'etos512@UNIQUE@.img',     'zip' => 'etos-512@UNIQUE@',  'unique' => 1, 'name' => '512k ROM image'),
	'@1024'            => array('filename' => 'etos1024k.img',           'zip' => 'etos-1024',         'unique' => 0, 'name' => '1024k ROM image, with all languages'),
	'aranym'           => array('filename' => 'emutos-aranym.img',       'zip' => 'etos-aranym',       'unique' => 0, 'name' => '512k ROM image, suitable for ARAnyM'),
	'firebee'          => array('filename' => 'etosfb@UNIQUE@.s19',      'zip' => 'etosfb@UNIQUE@',    'unique' => 1, 'name' => 'SREC file, to be flashed on the FireBee'),
	'firebee-prg'      => array('filename' => 'emutos.prg',              'zip' => 'etos-fb-prg',       'unique' => 0, 'name' => 'RAM tos for the FireBee'),
	'amiga'            => array('filename' => 'emutos-amiga.rom',        'zip' => 'etos-amiga',        'unique' => 0, 'name' => 'ROM for Amiga hardware'),
	'amigavampire'     => array('filename' => 'emutos-vampire.rom',      'zip' => 'etos-vampire',      'unique' => 0, 'name' => 'ROM for Amiga optimized for Vampire V2'),
	'v4sa'             => array('filename' => 'emutos-vampire-v4sa.rom', 'zip' => 'etos-v4sa',         'unique' => 0, 'name' => 'ROM for Amiga Vampire V4 Standalone'),
	'amigakd'          => array('filename' => 'emutos-kickdisk.adf',     'zip' => 'etos-kickdisk',     'unique' => 0, 'name' => 'EmuTOS as Amiga 1000 Kickstart disk'),
	'amigaflop'        => array('filename' => 'emutos.adf',              'zip' => 'etos-amiflop',      'unique' => 0, 'name' => 'EmuTOS RAM as Amiga boot floppy'),
	'amigaflopvampire' => array('filename' => 'emutos-vampire.adf',      'zip' => 'etos-vampflop',     'unique' => 0, 'name' => 'EmuTOS RAM as Amiga boot floppy optimized for Vampire V2'),
	'm548x-dbug'       => array('filename' => 'emutos-m548x-dbug.s19',   'zip' => 'etos-m548x-dbug',   'unique' => 0, 'name' => 'EmuTOS-RAM for dBUG on ColdFire Evaluation Boards'),
	'm548x-bas'        => array('filename' => 'emutos-m548x-bas.s19',    'zip' => 'etos-m548x-bas',    'unique' => 0, 'name' => 'EmuTOS for BaS_gcc on ColdFire Evaluation Boards'),
	'm548x-prg'        => array('filename' => 'emutos.prg',              'zip' => 'etos-m548x-prg',    'unique' => 0, 'name' => 'RAM tos for ColdFire Evaluation Boards with BaS_gcc'),
	'prg'              => array('filename' => 'emutos@UNIQUE@.prg',      'zip' => 'etos-prg@UNIQUE@',  'unique' => 1, 'name' => 'emutos.prg, a RAM tos'),
	'flop'             => array('filename' => 'emutos@UNIQUE@.st',       'zip' => 'etos-flop@UNIQUE@', 'unique' => 1, 'name' => 'Bootable floppy with RAM tos'),
	'pak3'             => array('filename' => 'etospak3@UNIQUE@.img',    'zip' => 'etos-pak3@UNIQUE@', 'unique' => 1, 'name' => '256k ROM, suitable for PAK/3 systems'),
	'cart'             => array('filename' => 'etoscart.img',            'zip' => 'etos-cart',         'unique' => 0, 'name' => 'EmuTOS as a diagnostic cartridge'),
	'lisaflop'         => array('filename' => 'emutos.dc42',             'zip' => 'etos-lisa@UNIQUE@', 'unique' => 1, 'name' => 'EmuTOS RAM as Apple Lisa boot floppy'),
);

function _redirect($message, $url = NULL)
{
    $_SESSION['sess_message'] = $message;
    if ($url)
    {
        header("Location: " . $url);
    } else if (isset($_SERVER['HTTP_REFERER']))
    {
        header("Location: " . $_SERVER['HTTP_REFERER']);
    } else
    {
        header("Location: " . 'index.php');
    }
    if (ob_get_level() > 0)
    {
		ob_end_clean();
	}
    exit();
}

function names($a)
{
	return $a['name'];
}

function setting(string $name, $select = null)
{
	$setting = "id=\"" . $name . "\" name=\"" . $name . "\"";
	if (isset($_GET[$name]))
		$_SESSION['settings'][$name] = $_GET[$name];
	if (!isset($_SESSION['settings'][$name]))
		$_SESSION['settings'][$name] = '';
	if ($select)
	{
		$setting .= ">\n";
		foreach ($select as $key => $value)
		{
			$setting .= "<option value=\"$key\"";
			if ($_SESSION['settings'][$name] === $key)
				$setting .= " selected=\"selected\"";
			$setting .= ">$value</option>\n";
		}
		$setting .= "</select>\n";
	} else
	{
		$setting .= " value=\"" . $_SESSION['settings'][$name] . "\"";
	}
	echo $setting;
}

function bool_setting()
{
	return array('' => 'default', 'off' => 'disable', 'on' => 'enable');
}

?>
