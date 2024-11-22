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

<!--
 ********************************************************
 *  H A R D W A R E   S U P P O R T   S E C T I O N     *
 ********************************************************
-->

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
<select <?php setting('conf_with_advanced_cpu', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_ADVANCED_CPU to 1 to enable support for 68010-68060
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Apollo:</td>
<td>
<select <?php setting('conf_with_apollo_68080', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_APOLLO_68080 to 1 to enable support for Apollo 68080 CPU
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>ST MMU:</td>
<td>
<select <?php setting('conf_with_st_mmu', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_ST_MMU to 1 to enable support for ST MMU
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>TT MMU:</td>
<td>
<select <?php setting('conf_with_tt_mmu', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_TT_MMU to 1 to enable support for TT MMU
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Falcon MMU:</td>
<td>
<select <?php setting('conf_with_falcon_mmu', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_FALCON_MMU to 1 to enable support for Falcon MMU
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>BLITTER:</td>
<td>
<select <?php setting('conf_with_blitter', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_BLITTER to 1 to enable blitter support
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>MONSTER:</td>
<td>
<select <?php setting('conf_with_monster', bool_setting()); ?>
</td>
<td>
Define CONF_WITH_MONSTER to enable detection and usage of ST/STE
MonSTer expansion card
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>MAGNUM:</td>
<td>
<select <?php setting('conf_with_magnum', bool_setting()); ?>
</td>
<td>
Define CONF_WITH_MAGNUM to enable detection and usage of Magnum
RAM expansion card
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>SCSI:</td>
<td>
<select <?php setting('conf_with_scsi', bool_setting()); ?>
</td>
<td>
Define CONF_WITH_SCSI to activate SCSI support
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>IDE:</td>
<td>
<select <?php setting('conf_with_ide', bool_setting()); ?>
</td>
<td>
Define CONF_WITH_IDE to activate IDE support
</td>
</tr>

<tr><td>&nbsp;</td><td></td><td></td></tr>
<tr><td>&nbsp;</td><td></td><td></td></tr>
<tr><td>&nbsp;</td><td></td><td></td></tr>

<!--
 ********************************************************
 *  S O F T W A R E   S E C T I O N   -   B I O S       *
 ********************************************************
-->

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
<tr><td>Memory test:</td>
<td>
<select <?php setting('conf_with_memory_test', bool_setting()); ?>
</td>
<td>
set CONF_WITH_MEMORY_TEST to 1 to do a memory test during a cold boot
</td>
</tr>

<!--
 ********************************************************
 *  S O F T W A R E   S E C T I O N   -   A E S         *
 ********************************************************
-->

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Color icons:</td>
<td>
<select <?php setting('conf_with_colour_icons', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_COLOUR_ICONS to 1 to enable support for colour icons,
as in Atari TOS 4
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>graf_mouse:</td>
<td>
<select <?php setting('conf_with_graf_mouse_extension', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_GRAF_MOUSE_EXTENSION to 1 to include AES support for
graf_mouse() modes M_SAVE, M_RESTORE, M_PREVIOUS.
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Window colors:</td>
<td>
<select <?php setting('conf_with_window_colours', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_WINDOW_COLOURS to 1 to include AES support for managing
window element colours.  Management is via modes WF_COLOR/WF_DCOLOR
in wind_get()/wind_set().
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>3D Objects:</td>
<td>
<select <?php setting('conf_with_3d_objects', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_3D_OBJECTS to 1 to enable support for 3D objects,
as in Atari TOS 4
</td>
</tr>

<!--
 ********************************************************
 *  S O F T W A R E   S E C T I O N   -   G E M D O S   *
 ********************************************************
-->

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
<tr><td>AES version:</td>
<td>
<select <?php setting('aes_version',
	array('' => 'default',
	      '0x0120' => 'AES 1.20, used by TOS v1.02',
	      '0x0140' => 'AES 1.40, used by TOS v1.04 & v1.62',
	      '0x0320' => 'AES 3.20, used by TOS v2.06 & v3.06',
	      '0x0330' => 'AES 3.30, indicates the availability of wind_set(WF_COLOR/WF_DCOLOR)',
	      '0x0340' => 'AES 3.40, used by TOS v4.04')); ?>
</td>
<td>
Define the AES version here. If unset, the value depends on features that are set
within the software section.<br />
Do not change this arbitrarily, as each value implies the presence or
absence of certain AES functions ... but note that we currently define
AES 3.30/3.40 even though we do not (yet) support certain menu_xxx()
functions that were introduced in AES 3.30.
</td>
</tr>

<!--
 ********************************************************
 *  S O F T W A R E   S E C T I O N   -   V D I         *
 ********************************************************
-->

<tr><td colspan="3"><hr /></td></tr>
<tr><td>VDI 16-bit:</td>
<td>
<select <?php setting('conf_with_vdi_16bit', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_VDI_16BIT to 1 to include VDI support for the Falcon&apos;s
16-bit graphics modes.
</td>
</tr>

<!--
 ************************************************************************
 *  S O F T W A R E   S E C T I O N   -   3 R D   P A R T Y   A P I     *
 ************************************************************************
-->

<tr><td colspan="3"><hr /></td></tr>
<tr><td>SCSI Driver:</td>
<td>
<select <?php setting('conf_with_scsi_driver', bool_setting()); ?>
</td>
<td>
Set CONF_WITH_SCSI_DRIVER to 1 to activate support for the SCSI driver
API, which allows user programs to issue SCSI-style commands directly
to devices. See the <a href="https://tho-otto.de/hypview/hypview.cgi?url=%2Fhyp%2Fscsidrv_en.hyp">documentation by Steffen Engel</a> for more details.
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Cartridge Debug Print:</td>
<td>
<select <?php setting('cartridge_debug_print', bool_setting()); ?>
</td>
<td>
Set CARTRIDGE_DEBUG_PRINT to 1 to redirect debug prints to the
cartridge port. A character 'c' is encoded into address lines
A8-A1 by performing a read access to the upper half of the cartridge
address space 0xFB0xxx, which is decoded to the /ROM3 signal.
This has the advantage of being always available since it does not
require initialization of a peripheral.
</td>
</tr>


<!--
 ********************************************************
 *  E M U L A T O R   S E C T I O N                     *
 ********************************************************
-->

<tr><td>&nbsp;</td><td></td><td></td></tr>
<tr><td>&nbsp;</td><td></td><td></td></tr>
<tr><td>&nbsp;</td><td></td><td></td></tr>

<tr><td colspan="3"><hr /></td></tr>
<tr>
<td colspan="3">
Emulator section:
</td>
<td></td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Use STOP instruction:</td>
<td>
<select <?php setting('use_stop_insn', bool_setting()); ?>
</td>
<td>
Set this to 1 if your emulator is capable of emulating properly the
STOP opcode (used to reduce host CPU burden during loops).  Set to
zero for all emulators which do not properly support the STOP opcode.
</td>
</tr>

<tr><td colspan="3"><hr /></td></tr>
<tr><td>Single FAT support:</td>
<td>
<select <?php setting('with_1fat_support', bool_setting()); ?>
</td>
<td>
Set this to 1 to enable support for filesystems with
only one file allocation table (FAT) instead of the usual two FATs.
<br />
This is disabled by default because all versions of Atari TOS assume
two FATs. There are erroneously mastered disks that claim to have a
single FAT, but in reality have two. For compatibility with Atari TOS
EmuTOS has to assume two FATs by default.
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
