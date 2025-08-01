<?php

if (isset($_SERVER['REMOTE_ADDR']))
{
	session_start();
}
$_SESSION['settings'] = $_POST;
include('vars.php');

$topdir = getcwd();

/* assumes cross toolchain is installed in <wwwdir>/usr */
if (isset($_SERVER['DOCUMENT_ROOT']))
{
	putenv("PATH=" . $_SERVER['DOCUMENT_ROOT'] . "/usr/bin:" . getenv("PATH"));
}


function send_headers()
{
	global $zip;
	
	header('Content-Description: File Transfer');
	header('Content-Type: application/x-zip-compressed');
	header('Content-Transfer-Encoding: binary');
	header('Expires: 0');
	header('Cache-Control: must-revalidate');
	header('Pragma: public');
	header('Content-Disposition: attachment; filename="'.basename($zip).'"');
}


function is_cli()
{
	return substr(php_sapi_name(), 0, 3) == 'cli';
}
	

function error_handler(int $errno, string $errmsg, string $errfile, int $errline, array $errcontext = []): bool
{
	global $log;
	global $last_error_message;
	fputs($log, "$errmsg\n");
	/* error_get_last() returns null with custom error handler being set */
	$last_error_message = $errmsg;
	return true;
}


function log_output($str)
{
	return '';
}


function get_options()
{
	global $emutosversion;
	global $country;
	global $romversion;
	global $targets;

	$romversion = '';
	$country = '';
	if (is_cli())
	{
		for ($i = 1; $i < $_SERVER["argc"]; $i++)
		{
			switch($_SERVER["argv"][$i])
			{
			case "--version":
				$i++;
				$emutosversion = $_SERVER["argv"][$i];
				break;
			case "--country":
				$i++;
				$country = $_SERVER["argv"][$i];
				break;
			case "--romversion":
				$i++;
				$romversion = $_SERVER["argv"][$i];
				break;
			}
		}
	} else
	{
		if (isset($_POST['emutosversion']))
		{
			$emutosversion = $_POST['emutosversion'];
		}
		if (isset($_POST['country']))
		{
			$country = $_POST['country'];
		}
		$romversion = isset($_POST['romversion']) ? $_POST['romversion'] : '';
	}
	if ($emutosversion == '')
	{
		$emutosversion = 'snapshot';
	}
	if (!isset($targets[$romversion]))
	{
		echo("unknown version $romversion");
		return false;
	}
	return true;
}


function onoff($s) : string
{
	if ($s == 'on')
		return "1";
	return "0";
}


function compile_emutos()
{
	global $emutosversion;
	global $country;
	global $romversion;
	global $zip;
	global $img_filename;
	global $exitcode;
	global $targets;
	global $commit;
	global $last_error_message;
	
	$retval = false;
	$exitcode = 1;
	
	$fp = fopen('localconf.h', 'w');
	if (!is_resource($fp))
	{
		error_log($last_error_message);
	} else
	{
		$target = str_replace("@", "", $romversion);
		fprintf($fp, "/*\n");
		fprintf($fp, " * version: $emutosversion\n");
		if ($emutosversion == 'snapshot')
			fprintf($fp, " * commit: $commit\n");
		fprintf($fp, " * target: $target\n");
		if ($targets[$romversion]['unique'])
			fprintf($fp, " * country: $country\n");
		fprintf($fp, " */\n\n");
		
		if ($_POST['conf_with_advanced_cpu'] != '')
			fprintf($fp, "#define CONF_WITH_ADVANCED_CPU %s\n", onoff($_POST['conf_with_advanced_cpu']));
		if ($_POST['conf_with_apollo_68080'] != '')
			fprintf($fp, "#define CONF_WITH_APOLLO_68080 %s\n", onoff($_POST['conf_with_apollo_68080']));
		if ($_POST['conf_with_st_mmu'] != '')
			fprintf($fp, "#define CONF_WITH_ST_MMU %s\n", onoff($_POST['conf_with_st_mmu']));
		if ($_POST['conf_with_tt_mmu'] != '')
			fprintf($fp, "#define CONF_WITH_TT_MMU %s\n", onoff($_POST['conf_with_tt_mmu']));
		if ($_POST['conf_with_falcon_mmu'] != '')
			fprintf($fp, "#define CONF_WITH_FALCON_MMU %s\n", onoff($_POST['conf_with_falcon_mmu']));
		if ($_POST['conf_with_blitter'] != '')
			fprintf($fp, "#define CONF_WITH_BLITTER %s\n", onoff($_POST['conf_with_blitter']));
		if ($_POST['conf_with_monster'] != '')
			fprintf($fp, "#define CONF_WITH_MONSTER %s\n", onoff($_POST['conf_with_monster']));
		if ($_POST['conf_with_magnum'] != '')
			fprintf($fp, "#define CONF_WITH_MAGNUM %s\n", onoff($_POST['conf_with_magnum']));
		if ($_POST['conf_with_scsi'] != '')
			fprintf($fp, "#define CONF_WITH_SCSI %s\n", onoff($_POST['conf_with_scsi']));
		if ($_POST['conf_with_ide'] != '')
			fprintf($fp, "#define CONF_WITH_IDE %s\n", onoff($_POST['conf_with_ide']));
		if ($_POST['conf_with_nova'] != '')
			fprintf($fp, "#define CONF_WITH_NOVA %s\n", onoff($_POST['conf_with_nova']));
		
		if ($_POST['conf_with_vdi_16bit'] != '')
			fprintf($fp, "#define CONF_WITH_VDI_16BIT %s\n", onoff($_POST['conf_with_vdi_16bit']));

		if ($_POST['initinfo_duration'] != '')
			fprintf($fp, "#define INITINFO_DURATION %d\n", $_POST['initinfo_duration']);
		if ($_POST['conf_with_memory_test'] != '')
			fprintf($fp, "#define CONF_WITH_MEMORY_TEST %d\n", $_POST['conf_with_memory_test']);
		if ($_POST['conf_with_colour_icons'] != '')
			fprintf($fp, "#define CONF_WITH_COLOUR_ICONS %s\n", onoff($_POST['conf_with_colour_icons']));
		if ($_POST['conf_with_graf_mouse_extension'] != '')
			fprintf($fp, "#define CONF_WITH_GRAF_MOUSE_EXTENSION %s\n", onoff($_POST['conf_with_graf_mouse_extension']));
		if ($_POST['conf_with_window_colours'] != '')
			fprintf($fp, "#define CONF_WITH_WINDOW_COLOURS %s\n", onoff($_POST['conf_with_window_colours']));
		if ($_POST['conf_with_3d_objects'] != '')
			fprintf($fp, "#define CONF_WITH_3D_OBJECTS %s\n", onoff($_POST['conf_with_3d_objects']));
		if ($_POST['tos_version'] != '')
			fprintf($fp, "#define TOS_VERSION %s\n", $_POST['tos_version']);
		if ($_POST['gemdos_version'] != '')
			fprintf($fp, "#define GEMDOS_VERSION %s\n", $_POST['gemdos_version']);
		if ($_POST['aes_version'] != '')
			fprintf($fp, "#define AES_VERSION %s\n", $_POST['aes_version']);
		if ($_POST['conf_with_scsi_driver'] != '')
			fprintf($fp, "#define CONF_WITH_SCSI_DRIVER %s\n", onoff($_POST['conf_with_scsi_driver']));
		if ($_POST['conf_with_xhdi'] != '')
			fprintf($fp, "#define CONF_WITH_XHDI %s\n", onoff($_POST['conf_with_xhdi']));

		if ($_POST['use_stop_insn'] != '')
			fprintf($fp, "#define USE_STOP_INSN_TO_FREE_HOST_CPU %s\n", onoff($_POST['use_stop_insn']));
		if ($_POST['with_1fat_support'] != '')
			fprintf($fp, "#define CONF_WITH_1FAT_SUPPORT %s\n", onoff($_POST['with_1fat_support']));
		if ($_POST['cartridge_debug_print'] != '')
			fprintf($fp, "#define CARTRIDGE_DEBUG_PRINT %s\n", onoff($_POST['cartridge_debug_print']));

		fclose($fp);
		
		$output = array();
		exec("make clean 2>&1");
		$cmd = "make -j2 " . $target;
		if ($targets[$romversion]['unique'])
			$cmd .= " UNIQUE=$country";
		$cmd .= " 2>&1";
		exec("$cmd", $output, $exitcode);
		
		if ($exitcode != 0)
		{
			echo("make exited with code $exitcode:\n\n");
			echo(implode("\n", $output));
			echo("\n\n");
			echo("# Configuration used:\n");
			echo("\n\n");
			echo(file_get_contents("localconf.h"));
			echo("\n# end config\n\n");
		} else
		{
			$output = array();
			exec("zip -j $zip $img_filename localconf.h 2>&1", $output, $exitcode);
			if ($exitcode != 0)
			{
				echo("zip exited with code $exitcode:\n\n");
				echo(implode("\n", $output));
			} else
			{
				$retval = true;
			}
		}
	}

	return $retval;
}


function safe_file_exists(string $filename) : bool
{
	$report = error_reporting();
	error_reporting($report & ~E_WARNING);
	$exists = file_exists($filename);
	error_reporting($report);
	return $exists;
}


function safe_unlink(string $filename)
{
	$report = error_reporting();
	error_reporting($report & ~E_WARNING);
	unlink($filename);
	error_reporting($report);
}


function fetch_source()
{
	global $emutosversion;
	global $romversion;
	global $country;
	global $targets;
	global $versions;
	global $github;
	global $github_releases;
	global $tmpdir;
	global $builddir;
	global $topdir;
	global $zip;
	global $img_filename;
	global $log;
	global $commit;
	global $exitcode;
	global $last_error_message;

	$source_archive = $versions[$emutosversion]['archive'];
	$top_dir = 'emutos-' . $source_archive;
	$source_archive .= '.tar.gz';
	$local_archive = $topdir . '/' . $source_archive;

	$tmpdir = tempnam(".", 'tmp');
	safe_unlink($tmpdir);
	if (!mkdir($tmpdir) || !chdir($tmpdir))
	{
		echo("chdir $tmpdir failed\n");
		return false;
	}

	$output = array();
	if ($emutosversion == 'snapshot')
	{
		$exitcode = 1;
		exec("git clone ${github}.git $top_dir 2>&1", $output, $exitcode);
		if ($exitcode != 0)
		{
			echo("cloning $github failed\n");
			echo(implode("\n", $output));
			chdir($topdir);
			rmdir($tmpdir);
			return false;
		}
		$builddir = $tmpdir . "/" . $top_dir;
	} else
	{
		if (!safe_file_exists($local_archive))
		{
			$archive = file_get_contents($github_releases . $source_archive);
			if (!$archive)
			{
				echo("downloading $github_releases$source_archive failed\n");
				chdir($topdir);
				rmdir($tmpdir);
				return false;
			}
			if (file_put_contents($local_archive, $archive) === false)
			{
				echo("error writing $source_archive\n");
				chdir($topdir);
				rmdir($tmpdir);
				return false;
			}
		}
	
		$exitcode = 1;
		exec("tar -xf $local_archive 2>&1", $output, $exitcode);
		if ($exitcode != 0)
		{
			echo("extracting $source_archive failed\n");
			echo(implode("\n", $output));
			chdir($topdir);
			rmdir($tmpdir);
			return false;
		}
		$builddir = $tmpdir . "/" . $top_dir;
	}
	fprintf($log, "builddir: $builddir\n");
	if (!chdir($builddir))
	{
		echo("$builddir does not exist\n");
		chdir($topdir);
		rmdir($tmpdir);
		return false;
	}
	if ($emutosversion == 'snapshot')
	{
		$commit = exec("git rev-list --abbrev-commit --max-count=1 HEAD");
		/*
		 * while we are at it, recreate the versions.php
		 */
		$tags = array();
		exec("git tag -l 'VERSION_*' | sort -t '_' -V -k 2,5 -r", $tags);
		$fp = fopen("$topdir/versions.php", "w");
		if (!is_resource($fp))
		{
			error_log("cannot create $topdir/versions.php: $last_error_message");
			return false;
		}
		fprintf($fp, "<" . "?php\n");
		fprintf($fp, "\$versions = array(\n");
		fprintf($fp, "\t'snapshot' => array('name' => 'Current snapshot', 'archive' => 'master'),\n");
		foreach ($tags as $version)
		{
			$dotted = str_replace("_", ".", str_replace("VERSION_", "", $version));
			fprintf($fp, "\t'%s' => array('name' => 'Release %s', 'archive' => '%s'),\n", $dotted, $dotted, $version);
		}
		fprintf($fp, ");\n");
		fprintf($fp, "?" . ">\n");
		fclose($fp);
	} else
	{
		$commit = '';
	}
	fprintf($log, "version: $emutosversion $commit\n");
		
	$img_filename = str_replace("@UNIQUE@", $country, $targets[$romversion]['filename']);
	$zip = str_replace("@UNIQUE@", $country, $targets[$romversion]['zip'] . '.zip');
	safe_unlink($zip);

	return true;
}


/* main() */

$log = fopen('emutos.log', 'a');
flock($log, LOCK_EX);
$currdate = date('Y-m-d H:i:s');
fputs($log, "$currdate: start\n");
fprintf($log, "FROM: %s\n", isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : 'here');
fprintf($log, "POST: %s\n", var_export($_POST, true));
/* fprintf($log, "SERVER: %s\n", var_export($_SERVER, true)); */

set_error_handler('error_handler', E_ALL);

ob_start("log_output");

$builddir = '';
$retval = get_options();

if ($retval)
	$retval = fetch_source();

if ($retval)
	$retval = compile_emutos();

$compile_output = ob_get_contents();
ob_end_clean();

/* fputs($log, "$compile_output\n"); */

if ($retval)
{
	if (!safe_file_exists($zip))
	{
		$compile_output .= "no such file: $zip\n";
		$retval = false;
	} else
	{
		if (headers_sent($file, $line))
		{
			error_log("headers already sent at $file $line");
		} else
		{
			send_headers();
			header('Content-Length: ' . filesize($zip));
		}
		flush();
		if (readfile($zip) === false)
		{
			$compile_output .= "error reading $zip\n";
			$retval = false;
		}
	}
}
if (!$retval)
{
	$errfile = fopen("$topdir/errors.log", 'a');
	if (is_resource($errfile))
	{
		fputs($errfile, "$currdate: start\n");
		fprintf($errfile, "FROM: %s\n", isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : 'here');
		fprintf($errfile, "POST: %s\n", var_export($_POST, true));
		fprintf($errfile, "FAILED:\n");
		fputs($errfile, "$compile_output\n\n");
		fclose($errfile);
	}
}

if ($builddir != '')
{
	exec("rm -rf $builddir");
	rmdir($tmpdir);
}

$currdate = date('Y-m-d H:i:s');
fprintf($log, "$currdate: end (%s)\n\n", $retval ? "success" : "failed");


fclose($log);

if (!$retval)
{
	_redirect("error compiling EmuTOS:\n\n" . $compile_output);
}

?>
