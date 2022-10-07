"use strict;"

var curmenu;
window.onclick = function()
{
  hideCurMenu();
}

function showMenu (id) {
  if (curmenu)
    hideMenu(curmenu);
  var a = document.getElementById(id + '_content');
  if (a) {
    a.style.display = 'block';
  }
  curmenu = id;
}
function hideMenu (id) {
  var a = document.getElementById(id);
  if (a)
    a.checked = '';
}
function hideCurMenu () {
  // hideMenu(curmenu);
}
