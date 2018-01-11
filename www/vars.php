<?php

$linguas = array();

$languages = array(
	'en' => array('en' => 'English',   'html' => 'en-US', 'flag' => 'uk.gif', 'native' => 'English'),
	'de' => array('en' => 'German',    'html' => 'de-DE', 'flag' => 'de.gif', 'native' => 'Deutsch'),
	'fr' => array('en' => 'French',    'html' => 'fr-FR', 'flag' => 'fr.gif', 'native' => 'Fran&#231;ais'),
	'es' => array('en' => 'Spanish',   'html' => 'es-ES', 'flag' => 'es.gif', 'native' => 'Espa&#241;ol'),
	'it' => array('en' => 'Italian',   'html' => 'it-IT', 'flag' => 'it.gif', 'native' => 'Italiano'),
	'en_GB' => array('en' => 'English',   'html' => 'en-GB', 'flag' => 'uk.gif', 'native' => 'English'),
	'sv' => array('en' => 'Swedish',   'html' => 'sv-SV', 'flag' => 'se.gif', 'native' => 'Svenska'),
	'de_CH' => array('en' => 'Swiss (German)', 'html' => 'de-CH', 'flag' => 'ch.gif', 'native' => 'Schweizerisch (Deutsch)'),
	'fr_CH' => array('en' => 'Swiss (French)', 'html' => 'fr-CH', 'flag' => 'ch.gif', 'native' => 'Suisse (Fran\207ais)'),
	'tr' => array('en' => 'Turkish',   'html' => 'tr-TR', 'flag' => 'tr.gif', 'native' => 'T&#252;rk&#231;e'),
	'fi' => array('en' => 'Finnish',   'html' => 'fi-FI', 'flag' => 'fi.gif', 'native' => 'Suomi'),
	'nn' => array('en' => 'Norwegian', 'html' => 'nn-NO', 'flag' => 'no.gif', 'native' => 'Norsk Nynorsk'),
	'nb' => array('en' => 'Norwegian', 'html' => 'nb-NO', 'flag' => 'no.gif', 'native' => 'Norsk Bokm&#xe5;l'),
	'da' => array('en' => 'Danish',    'html' => 'da-DK', 'flag' => 'dk.gif', 'native' => 'Dansk'),
	'ar' => array('en' => 'Arabic',    'html' => 'ar-SA', 'flag' => 'sa.gif', 'native' => '&#x0627;&#x0644;&#x0639;&#x0631;&#x0628;&#x064a;&#x0629;'),
	'nl' => array('en' => 'Dutch',     'html' => 'nl-NL', 'flag' => 'nl.gif', 'native' => 'Nederlands'),
	'cs' => array('en' => 'Czech',     'html' => 'cs-CZ', 'flag' => 'cz.gif', 'native' => '&#268;esky'),
	'hu' => array('en' => 'Hungarian', 'html' => 'hu-HU', 'flag' => 'hu.gif', 'native' => 'Magyar'),
	'pl' => array('en' => 'Polish',    'html' => 'pl-PL', 'flag' => 'pl.gif', 'native' => 'Polski'),
	'lt' => array('en' => 'Lithuanian','html' => 'lt-LT', 'flag' => 'lt.gif', 'native' => 'Lietuvi&#x0173; kalba'),
	'ru' => array('en' => 'Russian',   'html' => 'ru-RU', 'flag' => 'ru.gif', 'native' => '&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081;'),
	'et' => array('en' => 'Estonian',  'html' => 'et-EE', 'flag' => 'ee.gif', 'native' => 'Eesti'),
	'be' => array('en' => 'Belarusian','html' => 'be-BY', 'flag' => 'by.gif', 'native' => '&#x0431;&#x0435;&#x043b;&#x0430;&#x0440;&#x0443;&#x0441;&#x043a;&#x0430;&#x044f; &#x043c;&#x043e;&#x0432;&#x0430;'),
	'uk' => array('en' => 'Ukrainian', 'html' => 'uk-UA', 'flag' => 'ua.gif', 'native' => '&#x0423;&#x043a;&#x0440;&#x0430;&#x0457;&#x043d;&#x0441;&#x044c;&#x043a;&#x0430;'),
	'sk' => array('en' => 'Slovak',    'html' => 'sk-SK', 'flag' => 'sk.gif', 'native' => 'Sloven&#x010d;ina'),
	'ro' => array('en' => 'Romanian',  'html' => 'ro-RO', 'flag' => 'ro.gif', 'native' => 'Rom&x00e2;n&#x0103;'),
	'bg' => array('en' => 'Bulgarian', 'html' => 'bg-BG', 'flag' => 'bg.gif', 'native' => '&#x0431;&#x044a;&#x043b;&#x0433;&#x0430;&#x0440;&#x0441;&#x043a;&#x0438;'),
	'sl' => array('en' => 'Slovenian', 'html' => 'sl-SI', 'flag' => 'si.gif', 'native' => 'Sloven&#x0161;&#x010d;ina'),
	'hr' => array('en' => 'Croatian',  'html' => 'hr-HR', 'flag' => 'hr.gif', 'native' => 'Hrvatski'),
	'sr' => array('en' => 'Serbian',   'html' => 'sr-RS', 'flag' => 'rs.gif', 'native' => '&#x0441;&#x0440;&#x043f;&#x0441;&#x043a;&#x0438; &#x0458;&#x0435;&#x0437;&#x0438;&#x043a;'),
	'sr_ME' => array('en' => 'Montenegrin',   'html' => 'sr-ME', 'flag' => 'me.gif', 'native' => '&#x0446;&#x0440;&#x043d;&#x043e;&#x0433;&#x043e;&#x0440;&#x0441;&#x043a;&#x0438;'),
	'mk' => array('en' => 'Macedonian','html' => 'mk-MK', 'flag' => 'mk.gif', 'native' => '&#x043c;&#x0430;&#x043a;&#x0435;&#x0434;&#x043e;&#x043d;&#x0441;&#x043a;&#x0438; &#x0458;&#x0430;&#x0437;&#x0438;&#x043a;'),
	'gr' => array('en' => 'Greek',     'html' => 'el-GR', 'flag' => 'gr.gif', 'native' => '&#917;&#955;&#955;&#951;&#957;&#953;&#954;&#940;'), /* deprecated; for compatibility */
	'el' => array('en' => 'Greek',     'html' => 'el-GR', 'flag' => 'gr.gif', 'native' => '&#917;&#955;&#955;&#951;&#957;&#953;&#954;&#940;'),
	'lv' => array('en' => 'Latvian',   'html' => 'lv-LV', 'flag' => 'lv.gif', 'native' => 'Latvie&#x0161;u'),
	'he' => array('en' => 'Hebrew',    'html' => 'he-IL', 'flag' => 'il.gif', 'native' => '&#x05e2;&#x05b4;&#x05d1;&#x05e8;&#x05b4;&#x05d9;&#x05ea;'),
	'af' => array('en' => 'Afrikaans', 'html' => 'af-ZA', 'flag' => 'za.gif', 'native' => 'Afrikaans'),
	'pt' => array('en' => 'Portuguese','html' => 'pt-PT', 'flag' => 'pt.gif', 'native' => 'Portugu&#x00ea;s'),
	'nl_BE' => array('en' => 'Dutch (Belgian)','html' => 'nl-BE', 'flag' => 'be.gif', 'native' => 'Nederlands (Belgian)'),
	'ja' => array('en' => 'Japanese',  'html' => 'ja-JP', 'flag' => 'jp.gif', 'native' => '&#x65e5;&#x672c;&#x8a9e;'),
	'zh' => array('en' => 'Chinese',   'html' => 'zh-CN', 'flag' => 'cn.gif', 'native' => '&#x4e2d;&#x6587;'),
	'ko' => array('en' => 'Korean',    'html' => 'ko-KO', 'flag' => 'ko.gif', 'native' => '&#xd55c;&#xad6d;&#xc5b4;'),
	'vi' => array('en' => 'Vietnamese','html' => 'vi-VN', 'flag' => 'vn.gif', 'native' => 'Ti&#x1ebf;ng Vi&#x1ec7;t'),
	'hi' => array('en' => 'Hindi',     'html' => 'hi-IN', 'flag' => 'in.gif', 'native' => '&#x0939;&#x093f;&#x0902;&#x0926;&#x0940;'),
	'fa' => array('en' => 'Farsi',     'html' => 'fa-IR', 'flag' => 'ir.gif', 'native' => '&#x0641;&#x0627;&#x0631;&#x0633;&#x06cc;'),
	'mn' => array('en' => 'Mongolian', 'html' => 'mn-MN', 'flag' => 'mn.gif', 'native' => '&#x041c;&#x043e;&#x043d;&#x0433;&#x043e;&#x043b; &#x0445;&#x044d;&#x043b;'),
	'ne' => array('en' => 'Nepali',    'html' => 'ne-NP', 'flag' => 'np.gif', 'native' => '&#x0928;&#x0947;&#x092a;&#x093e;&#x0932;&#x0940;'),
	'lo' => array('en' => 'Lao',       'html' => 'lo-LA', 'flag' => 'la.gif', 'native' => '&#x0ea5;&#x0eb2;&#x0ea7;'),
	'km' => array('en' => 'Khmer',     'html' => 'km-KH', 'flag' => 'kh.gif', 'native' => '&#x1797;&#x17b6;&#x179f;&#x17b6;&#x1781;&#x17d2;&#x1798;&#x17c2;&#x179a;'),
	'id' => array('en' => 'Indonesian','html' => 'id-ID', 'flag' => 'id.gif', 'native' => 'bahasa Indonesia'),
	'bn' => array('en' => 'Bengali',   'html' => 'bn-BD', 'flag' => 'bd.gif', 'native' => '&#x09ac;&#x09be;&#x0999;&#x09be;&#x09b2;&#x09bf;'),
);

function read_linguas($filename, $warn = 0)
{
	global $linguas;
	global $languages;
	$linguas = array();
	$linguas[] = array('lang' => 'en', 'charset' => 'atarist');
	$in = fopen($filename, 'r');
	while (($line = fgets($in)) !== false)
	{
		$line = rtrim($line, "\x0D\x0A");
		if (preg_match('/^#.*/', $line, $matches)) continue;
		if (preg_match('/^([a-z]+) (.*)/', $line, $matches))
		{
			$linguas[] = array('lang' => $matches[1], 'charset' => $matches[2]);
		}
	}
	fclose($in);
	foreach ($linguas as $lang)
	{
		$lang = $lang['lang'];
		$trans = $languages[$lang];
		if (!$trans)
		{
			$trans = array('en' => $lang, 'native' => $lang, 'flag' => $lang . '.gif');
			$languages[$lang] = $trans;
			if ($warn)
			{
				echo "warning: unknown language $lang\n";
			}
		}
	}
}

?>
