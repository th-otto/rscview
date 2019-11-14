/* This file was automatically generated by rscview.cgi version 1.01 */

"use strict;"
function showPopup (id) {
  var a = document.getElementById(id + '_content');
  if (a) {
    if (a.style.display == 'none' || a.style.display == '') {
      a.style.display = 'block';
    } else {
      a.style.display = 'none';
    }
  }
}
function hidePopup (id) {
  var a = document.getElementById(id + '_content');
  if (a)
    a.style.display = 'none';
}
function hideInfo () {
  var id = 'rsc_info';
  var a = document.getElementById(id + '_content');
  if (a)
    a.style.display = 'none';
}
function showInfo () {
  var id = 'rsc_info';
  var a = document.getElementById(id + '_content');
  if (a) {
    if (a.style.display == 'none' || a.style.display == '') {
      a.style.display = 'inline-block';
    } else {
      a.style.display = 'none';
    }
  }
}
function getQueryVariable(variable)
{
  var query = window.location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (decodeURIComponent(pair[0]) == variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return '';
}
var lang;
var languages = ['en', 'de'];
function getSupportedLanguage(l)
{
  for (var j = 0; j < languages.length; j++) {
    if (languages[j] == l) return l;
  }
  return '';
}
function getAcceptLanguage()
{
  if (window.navigator.languages) {
    for (var i = 0; i < window.navigator.languages.length; i++) {
      var l = window.navigator.languages[i].split('-')[0].split('_')[0];
      l = getSupportedLanguage(l);
      if (l != '') return l;
    }
  }
  return '';
}
function getLanguage()
{
  var l = getQueryVariable('lang').split('-')[0].split('_')[0];
  if (l == '') {
    l = getAcceptLanguage();
  }
  l = getSupportedLanguage(l);
  if (l == '')
    l = getSupportedLanguage(navigator.language);
  if (l == '')
    l = 'en';
  lang = l;
  var html = document.getElementsByTagName('html')[0];
  html.setAttribute('lang', lang);
  html.setAttribute('xml:lang', lang);
}
getLanguage();

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
  var a = document.getElementById(id + '_content');
  if (a)
    a.style.display = 'none';
}
function hideCurMenu () {
  hideMenu(curmenu);
}