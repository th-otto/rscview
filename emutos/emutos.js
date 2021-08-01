function changeVersion()
{
	var version = document.getElementById('emutosversion').value;
}

function changeRomVersion()
{
	var version = document.getElementById('romversion').value;
	var disabled = (version == '@192' || version == '@256' || version == '@512' || version == 'prg' || version == 'flop' || version == 'pak3') ? '' : 'disabled';
	document.getElementById('country').disabled = disabled;
}

function onload()
{
	changeVersion();
	changeRomVersion();
}
