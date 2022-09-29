<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
          "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en" lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
<title>RscView Home</title>
<style type="text/css">
body {
	margin:1em;
	padding:1em;
	background-color: #cccccc;
}
</style>
<script type="text/javascript">
function submitUrl(url)
{
	var f = document.getElementById('urlform');
	f.method = 'get';
	f.enctype = "application/x-www-form-urlencoded";
	var fileinput = document.getElementById('file');
	var urlinput = document.getElementById('url');
	fileinput.disabled = true;
	var oldvalue = urlinput.value;
	urlinput.value = url;
	f.submit();
	fileinput.disabled = false;
	urlinput.value = oldvalue;
}
function submitFile()
{
	var f = document.getElementById('urlform');
	f.method = 'post';
	f.enctype = "multipart/form-data";
	f.submit();
}
</script>
</head>
<body>
<div>
<h1 style="font-weight: bold;">
Resource File View Web Service<span style="font-size: 13pt"> - provided by <a href="http://www.tho-otto.de/">Thorsten Otto</a></span>
</h1>
<br/><br/>

<em>Want to browse .RSC files in an HTML browser?</em><br />
<br />

<form action="rscview.cgi" method="get" id="urlform" enctype="application/x-www-form-urlencoded">

<noscript>
<p><span style="color:red">
<b>Your browser does not support JavaScript.</b>
<br />
Some features will not work without JavaScript enabled.
</span>
<br />
<br /></p>
</noscript>

<table>

<tr style="vertical-align: top;">
<td>
<fieldset>
Type in URL of a .RSC file (it must be remotely accessible from that URL<br />
for example <a href="rscview.cgi?url=/rsc/orcs.rsc&amp;contents=1">http://www.tho-otto.de/rsc/orcs.rsc</a>):
<br />
<input type="text" id="url" name="url" size="60" tabindex="1" style="margin-top: 1ex;" />
<input id="submiturl" style="background-color: #cccccc; font-weight: bold; visibility: hidden;" type="button" value="View" onclick="submitUrl(document.getElementById('url').value);" />
<noscript>
<div id="submitnoscript"><span><input type="submit" style="background-color: #cccccc; font-weight: bold;" value="View" /></span></div>
</noscript>
<script type="text/javascript">
document.getElementById('submiturl').style.visibility="visible";
</script>
</fieldset>
<div id="uploadbox" style="display:none;">
<br />
<b>OR</b><br />
<br />
<fieldset>
Choose a .RSC file for upload <br />
<input type="file" id="file" name="file" size="60" accept=".rsc,.RSC,.rso,.RSO,.hrd,.HRD,.rsd,.RSD,.def,.DEF,.dfn,.DFN" style="margin-top: 1ex;" required="required" multiple="multiple"/>
<input id="submitfile" style="background-color: #cccccc; font-weight: bold;" type="button" value="View" onclick="submitFile();" /> <br />
Note: if you have a resource definition file (*.rso, *.hrd, *.rsm etc.),
you should select that along with the resource file.
</fieldset>
<br />
</div>
<script type="text/javascript">
document.getElementById('uploadbox').style.display="block";
</script>
<br />

<fieldset>
<table>
<tr><td style="padding-left: 2ex"><input type="checkbox" name="aes3d" value="1" /> Use 3d-effects<br /></td></tr>
<tr>
<td>
<input type="radio" name="index" value="0" checked="checked" />Show only first dialog
</td>
<td>
<input type="radio" name="index" value="-1" />Show all dialogs
</td>
</tr>
</table>
</fieldset>

</table>
</form>

</div>

</body>
</html>
