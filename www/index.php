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

<table>

<noscript>
<tr><td>
<p><span style="color:red">
<b>Your browser does not support JavaScript.</b>
<br />
Some features will not work without JavaScript enabled.
</span>
<br />
<br /></p>
</td></tr>
</noscript>

<tr style="vertical-align: top;">
<td>
<form action="rscview.cgi" method="get" id="urlform" enctype="application/x-www-form-urlencoded">
<fieldset>
Type in URL of a .RSC file (it must be remotely accessible from that URL<br />
for example <a href="rscview.cgi?url=/rsc/orcs.rsc">http://www.tho-otto.de/rsc/orcs.rsc</a>):
<br />
<input type="text" id="url" name="url" size="60" tabindex="1" style="margin-top: 1ex;"" />
<input id="submiturl" style="background-color: #cccccc; font-weight: bold;" type="submit" value="View" />
</fieldset>
</form>
</td></tr>

<tr style="vertical-align: top;">
<td>
<br />
<b>OR</b><br />
<br />
</td></tr>

<tr style="vertical-align: top;">
<td>
<form action="rscview.cgi" method="post" id="fileform" enctype="multipart/form-data">
<fieldset>
Choose a .RSC file for upload <br />
<input type="file" id="file" name="file" size="60" accept=".rsc,.RSC,.rso,.RSO,.hrd,.HRD,.rsd,.RSD,.def,.DEF,.dfn,.DFN" style="margin-top: 1ex;" required="required" multiple="multiple"/>
<input id="submitfile" style="background-color: #cccccc; font-weight: bold;" type="submit" value="View" />
</fieldset>
</form>
</td></tr>

</table>

</div>

</body>
</html>
