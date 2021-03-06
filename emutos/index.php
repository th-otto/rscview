<?php
if (isset($_SERVER['REMOTE_ADDR']))
{
	session_start();
}
include('vars.php');
?>

<!DOCTYPE html>
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>EmuTOS build service</title>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<meta name="keywords" content="ARAnyM, EmuTOS" />
<link rel="stylesheet" type="text/css" href="emutos.css" />
<script src="emutos.js"></script>
</head>

<body onload="onload();">
<div>

<hr />
<h1>EmuTOS custom building made easy</h1>
<hr />

<?php
	if (isset($_SESSION['sess_message']))
	{
		echo "<div class=\"error\">\n";
		echo "<pre>" . $_SESSION['sess_message'] . "</pre>\n";
		echo "</div>\n";
		unset($_SESSION['sess_message']);
	}
?>

<table>
<tr>
<td>
<form action="emutos.php" method="post" id="emutos" enctype="multipart/form-data">
<fieldset>
<table>

<tr>
<td>
Version:
</td>
<td colspan="2">
<select onchange="changeVersion();" <?php setting('emutosversion', array_map('names', $versions)); ?>
</td>
</tr>

<tr>
<td>
Output:
</td>
<td colspan="2">
<select onchange="changeRomVersion();" <?php setting('romversion', array_map('names', $targets)); ?>
</td>
</tr>

<tr>
<td>
Country:
</td>
<td colspan="2">
<select <?php setting('country', array_map('names', $countries)); ?>
</td>
</tr>

<tr><td>&nbsp;</td><td></td><td></td></tr>

<tr><td colspan="3"><hr /></td></tr>
<tr>
<td colspan="3">
Hardware support section:
</td>
<td></td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Advanced CPU:</td>
<td>
<select <?php setting('conf_with_advanced_cpu', array('' => 'default', 'off' => 'disable', 'on' => 'enable')); ?>
</td>
<td>
Set CONF_WITH_ADVANCED_CPU to 1 to enable support for 68010-68060
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Apollo:</td>
<td>
<select <?php setting('conf_with_apollo_68080', array('' => 'default', 'off' => 'disable', 'on' => 'enable')); ?>
</td>
<td>
Set CONF_WITH_APOLLO_68080 to 1 to enable support for Apollo 68080 CPU
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>ST MMU:</td>
<td>
<select <?php setting('conf_with_st_mmu', array('' => 'default', 'off' => 'disable', 'on' => 'enable')); ?>
</td>
<td>
Set CONF_WITH_ST_MMU to 1 to enable support for ST MMU
</td>
</tr>

<tr><td>&nbsp;</td><td></td><td></td></tr>

<tr><td colspan="3"><hr /></td></tr>
<tr>
<td colspan="3">
Software section:
</td>
<td></td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Init info duration:</td>
<td>
<input type="number" <?php setting('initinfo_duration'); ?> min="0" max="1024" style="width: 5em" /><br />
</td>
<td>
By default, the EmuTOS welcome screen (initinfo) is displayed for 3
seconds. On emulators, this is enough to read the text, and optionally
to press Shift to keep the screen displayed. But on real hardware, it
can take several seconds for the monitor to recover from stand-by mode,
so the welcome screen may never be seen. In such cases, it is wise to
increase the welcome screen duration.<br />
You can use the INITINFO_DURATION define to specifiy the welcome screen
duration, in seconds. If it is set to 0, the welcome screen will never
 be displayed.
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>TOS version:</td>
<td>
<select <?php setting('tos_version',
	array('' => 'default',
	      '0x104' => '1.04',
	      '0x206' => '2.06',
	      '0x306' => '3.06',
	      '0x404' => '4.04')); ?>
<td>
By default, we pretend to be TOS 2.06, as it is available as an update for
both ST and STe. On the other hand, TOS 3.x is only for TT and TOS 4.x
only for Falcon.
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>GEMDOS version:</td>
<td>
<select <?php setting('gemdos_version',
	array('' => 'default',
	      '0x1300' => '0.13, used by TOS v1.0 & v1.02',
	      '0x1500' => '0.15, used by TOS v1.04 & v1.06',
	      '0x1700' => '0.17, used by TOS v1.62',
	      '0x1900' => '0.19, used by TOS v2.01, v2.05, v3.01, v3.05',
	      '0x2000' => '0.20, used by TOS v2.06 & v3.06',
	      '0x3000' => '0.30, used by TOS v4.0x')); ?>
</td>
<td>
Define the GEMDOS version here: this number is returned by the GEMDOS
Sversion() function call. The value contains the minor version number
in the high-order byte, and the major version number in the low-order
byte.<br />
This does not have a well-defined purpose, although it could be checked
by programs to determine presence or absence of certain GEMDOS functions.
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>

<tr><td>&nbsp;</td><td></td><td></td></tr>

<tr>
<td>
<input id="runit" style="background-color: #cccccc; font-weight: bold;" type="submit" value="Create EmuTOS image" />
</td>
<td>
(takes about 15 sec)
</td>
<td></td>
</tr>

</table>
</fieldset>
</form>
</td>
</tr>

<tr><td>&nbsp;</td></tr>

</table>


<div style="text-align:center">
<p>
<a href="../index.html"> <img src="../images/home1.png" width="180" height="60" style="border:0" alt="Home" /></a>
</p>
</div>

<div style="text-align:center">
<p>
<a href="https://validator.w3.org/check?uri=referer"><img
        src="../images/valid-xhtml11.png" height="31" width="88"
        alt="Valid XHTML 1.1!" /></a>
</p>
</div>

</div>
</body>
</html>
