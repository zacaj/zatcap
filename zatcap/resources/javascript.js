function nodeFromHtml(html)
{
	var t=document.createElement('span');
	t.innerHTML=html;
	return t;
}