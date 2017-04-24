<?php
echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n";
echo "          \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
echo "<html xml:lang=\"en\" lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n";
echo "<head>\n";
echo "<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\" />\n";
echo "<pre>\n";
echo file_get_contents('github.log');
echo "</pre>\n";
echo "</body>\n";
echo "</html>\n";
?>
