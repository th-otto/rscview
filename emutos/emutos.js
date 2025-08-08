function changeVersion()
{
	var version = document.getElementById('emutosversion').value;
}

function changeRomVersion()
{
	var version = document.getElementById('romversion').value;
	// keep in sync with $targets[version].unique in vars.php
	var disabled = (version == '@192' || version == '@256' || version == '@512' || version == 'prg' || version == 'flop' || version == 'pak3' || version == 'lisaflop') ? '' : 'disabled';
	document.getElementById('country').disabled = disabled;
}

function onload()
{
	changeVersion();
	changeRomVersion();
}
