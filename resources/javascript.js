function init()
{
	updateTweetLength();
}
var columns=new Array();
function addColumn(n,name)
{
	n.zname=name;
	document.getElementById('columns').appendChild(n);
	document.getElementById(name).pendingTweets=new Array();
	columns.push(n);
}
function hideColumns(name)
{
	var i=0;
	if(!name)
		columns[i++].style.display="inline-block";
		
	for(;i<columns.length;i++)
		if(columns[i].zname==name)
			columns[i].style.display="inline-block";
		else
			columns[i].style.display="none";
}
function showColumns()
{
	for(var i=0;i<columns.length;i++)
		columns[i].style.display="inline-block";
}
var tweetContent="";
function tweetboxPaste(e)
{
	if (/image.*/.test(e.clipboardData.types) || /image.*/.test(e.clipboardData.types))
	{
		/*var clipboardBuffer;
       var clipboardData = event.clipboardData;
		var found=false;
		Array.prototype.forEach.call(clipboardData.types, function(type, i) {
          var file;
          if (found) {
            return;
          }
		  
          if (type.match(/image.*//*) || clipboardData.items[i].type.match(/image.*//*)) {
            file = clipboardData.items[i].getAsFile();
            var reader = new FileReader();
			  reader.onload=function(e)
			  {
			  cpp.debug("match2");
				var d=reader.result;
				cpp.debug("read");
				cpp.debug(d);
				clipboardBuffer=d;
				return found = true;
			  };
			 // cpp.debug("match");
			  reader.readAsBinaryString(file);
			//  cpp.debug("match3");
		  }
        });
		
		cpp.debug("c "+clipboardBuffer);
		var str=textbox.value.substr(0,textbox.selectionStart)+clipboardBuffer+textbox.value.substr(textbox.selectionStart);
		textbox.value=str;
		updateTweetLength();*/
        var textbox=document.getElementById('tweetbox');
		cpp.pasteImage(textbox.selectionStart);
		if (e.preventDefault) {
		  e.stopPropagation();
		  e.preventDefault();
		}
		e.returnValue = false;
		return false;
      }
	  
	  
	  
	/**/
	
}
function insertText(text,at)
{
	if(!at)
		at=textbox.selectionStart;
	var textbox=document.getElementById('tweetbox');
	var str=textbox.value.substr(0,at)+text+textbox.value.substr(at);
	textbox.value=str;
	updateTweetLength();
}
function tweetboxKeydown(e)
	{
		if(completebox)
		{		
			var c=completebox.children;
			if(e.keyCode==38 && c.length>1)
			{
				for(var i=1;i<c.length;i++)
				{
					if(c[i].className=="completeboxitemselected")
					{
						for(var j=i-1;j>=0;j--)
						{
							if(c[j].style.display!="none")
							{
								c[i].className="completeboxitem";
								c[j].className="completeboxitemselected";
								//c[j].scrollIntoView();
								break;
							}
						}
						e.preventDefault();
						break;
					}
				}
			}
			else if(e.keyCode==40 && c.length>1)
			{
				for(var i=0;i<c.length-2;i++)
				{
					if(c[i].className=="completeboxitemselected")
					{
						for(var j=i+1;j<c.length;j++)
						{
							if(c[j].style.display!="none")
							{
								c[i].className="completeboxitem";
								c[j].className="completeboxitemselected";
								//c[j].scrollIntoView();
								break;
							}
						}
						e.preventDefault();
						break;
					}
				}
			}
			else if(e.keyCode==13 && c.length>0)
			{
				for(var i=0;i<c.length;i++)
				{
					if(c[i].className=="completeboxitemselected")
					{
						e.preventDefault();
						insertCompletion(c[i].username);
						break;
					}
				}
			}
			else if(e.keyCode==27)
			{
				remove('completebox');
				e.preventDefault();
			}
		}
		else if(e.which == 13 && e.shiftKey) 
		{
			
		}
		else if (e.which == 13) 
		{
		   e.preventDefault(); //Stops enter from creating a new line
		   sendTweet();
		}
	}
var inReplyTo="";
function moveCaretToEnd(el) {
    if (typeof el.selectionStart == "number") {
        el.selectionStart = el.selectionEnd = el.value.length;
    } else if (typeof el.createTextRange != "undefined") {
        el.focus();
        var range = el.createTextRange();
        range.collapse(false);
        range.select();
    }
}
function isValidInUsername(c)
{
	if((c<48 || (c>57 && c<65) || (c>90 && c<97) || c>122) && c!=95)
	return false;
	return true;
}
function setReplyTo(id,author)
{
	var text=document.getElementById(id+'_text').innerHTML;
	inReplyTo=id;
	var to=new Array();
	//if(!globals || author!='@'+globals.username || author=='@'+username)
		to.push(author);
	for(var i=0;i<text.length;i++)
	{
		if(text.charAt(i)=='@')
		{
			var str='';
			for(var j=i;j<text.length;j++)
			{
				var c=text.charCodeAt(j)
				if(!isValidInUsername(c) && c!=64)
					break;
				str=str+text.charAt(j);
			}
			if(globals && str!='@'+globals.username && str.length>0 && to.indexOf(str)==-1)
				to.push(str);
			i=j;
		}
				
	}
	var i=0;
	if(to.length>1 && author=='@'+globals.username)
		i++;
	for(;i<to.length;i++)
		document.getElementById('tweetbox').value+=to[i]+" ";
	if(to.length>1 && author=='@'+globals.username)
		document.getElementById('tweetbox').value+=to[0]+" ";
	document.getElementById('tweetbox').focus();
	moveCaretToEnd(document.getElementById('tweetbox'));
	
}
function sendTweet(tweet)
{
	if(!tweet)
		tweet=document.getElementById('tweetbox').value;
	cpp.sendTweet(tweet,inReplyTo);
	document.getElementById('tweetbox').value="";
}
var completebox=null;
var usernames=new Array();
function updateTweetLength()
{
	document.getElementById('TweetLength').innerHTML=""+(140-twttr.txt.getTweetLength(document.getElementById('tweetbox').value));
	var textbox=document.getElementById('tweetbox');
	if(textbox.selectionStart!=0)
	{
		if(completebox)
		{
			var str=textbox.value.substr(textbox.value.lastIndexOf('@',textbox.selectionStart-1)+1,textbox.selectionStart);
			//cpp.debug(str);
			for(var i=0;i<str.length;i++)
				if(isValidInUsername(str.charCodeAt(i))==false)
				{
					remove('completebox');
					completebox=null;
					return;
				}
			var children=completebox.children;
			var visible=-1;
			var makeSelected=false;
			for(var i=0;i<children.length;i++)
			{
				if(children[i].username)
				{
					if(children[i].username.substr(0,str.length)==str)
					{
						children[i].style.display="block";
						if(makeSelected==true)
						{
							makeSelected=false;
							children[i].className="completeboxitemselected"
							//children[i].className.scrollIntoView();
						}
						visible=i;
					}
					else
					{
						children[i].style.display="none";
						if(children[i].className=="completeboxitemselected")
						{
							children[i].className="completeboxitem";
							makeSelected=true;
						}
					}
				}
			}
			
			if(visible==-1)
				completebox.style.display="none";
			else
			{
				completebox.style.display="block";
				if(makeSelected==true)
				{
					children[visible].className="completeboxitemselected"
					children[visible].scrollIntoView();
				}
			}
		}
		else if(textbox.value.charAt(textbox.selectionStart-1)=='@')
		{
			var positioner = new maxkir.CursorPosition(textbox, 3);
			var p=positioner.getPixelCoordinates();
		//cpp.debug(''+textbox.offsetHeight+','+p[1]);
			//document.getElementById('tweetboxcontainer').removeChild('completebox');
			remove('completebox');
			var n=nodeFromHtml("<div id='completebox' style='left: "+(p[0]+2)+"px; bottom:"+(textbox.offsetHeight-p[1]+4)+"px;'></div>");
			document.getElementById('tweetboxcontainer').appendChild(n);
			completebox=document.getElementById('completebox');
			for(var i=0;i<usernames.length;i++)
			{
				var no=nodeFromHtml("<div class='completeboxitem' onclick='insertCompletion(\""+usernames[i]+"\");'>"+usernames[i]+"</div>");
				no.username=usernames[i];
				if(i==0)
					no.className="completeboxitemselected";
				completebox.appendChild(no);
			}
		}
	}
	else if(completebox)
	{
		remove('completebox');
					completebox=null;
	}
		
}
var muteStr="";
function insertCompletion(text)
{
	var textbox=document.getElementById('tweetbox');
	//cpp.debug(text);
	var start=textbox.value.lastIndexOf('@',textbox.selectionStart-1)+1;
	//cpp.debug("2");
	var str=textbox.value.substr(0,start)+text+textbox.value.substr(textbox.selectionStart)+" ";
	//cpp.debug(str);
	textbox.value=str;
	updateTweetLength();
}
function nodeFromHtml(html)
{
	var t=document.createElement('div');
	t.innerHTML=html;
	return t.firstChild;
}
function addTweet(columnName,n)
{
	var a=document.getElementById(columnName).children;
	var tweetId=n.tweetid;
	if(a.length==0)
	{
		document.getElementById(columnName).insertBefore(n,null);
		//cpp.print("Inserted "+tweetId+" at index 0");
		return tweetId;
	}
	//cpp.print("not empty");
	var i;
	for(i=0;i<a.length;i++)
	{
		//cpp.debug("id: " +a[i].id+" ("+tweetId+")");
		if(a[i]!== undefined && tweetId>a[i].tweetid)
		{
			//cpp.print("Inserted "+tweetId+" at index "+i);
			document.getElementById(columnName).insertBefore(n,a[i]);
			return tweetId;
		}
		if(a[i].tweetid===undefined)
			cpp.debug("no id");
	}
	//cpp.print("Inserted "+tweetId+" at index(end) "+i);
			document.getElementById(columnName).insertBefore(n,null);
//	cpp.debug("insert at "+i);
	return tweetId;
}
function addTweetHtml(columnName,tweetHtml,tweetId)
{
	//cpp.print("Adding "+tweetId);
	var column=document.getElementById(columnName);
	var n=nodeFromHtml(tweetHtml);	
	n.tweetid=tweetId;
	if(column.scrollTop<10) 
	{
		addTweet(columnName,n);
		}
	else if(column.pendingTweets===undefined)
			cpp.debug("no pending tweets existing");
			else
			{
		column.pendingTweets.push(n);}
}
function removeTweet(columnName,id)
{
//cpp.debug(id+"_"+columnName);
	remove(id+"_"+columnName);
}
function checkScroll(columnName)
{
	var column=document.getElementById(columnName);
	if(column.scrollTop<10) 
	{
			//cpp.debug("pending tweets: "+column.pendingTweets.length);
		if(column.pendingTweets===undefined)
			cpp.debug("no pending tweets existing");
		else if(column.pendingTweets.length>0)
		{
			var oldFirst=column.firstChild;
			for(var i=0;i<column.pendingTweets.length;i++)
				addTweet(columnName,column.pendingTweets[i]);
			column.scrollTop=oldFirst.offsetTop;
		}
	}
}
function doFavorite(id,columnName)
{
	var favImg=document.getElementById(id+"_"+columnName+"_fav");
	if(favImg.src=="asset://resource/favorite.png" || favImg.src=="asset://resource/favorite_hover.png")
	{
		cpp.favorite(id);
		favImg.src="asset://resource/favorite_on.png";
	}
	else
	{
		cpp.unfavorite(id);
		favImg.src="asset://resource/favorite.png";
	}			
}
function doRetweet(id,columnName)
{
	var favImg=document.getElementById(id+"_"+columnName+"_rt");
	if(favImg.src=="asset://resource/retweet.png" || favImg.src=="asset://resource/retweet_hover.png")
	{
		cpp.retweet(id);
		favImg.src="asset://resource/retweet_on.png";
	}	
}
function doDelete(id,columnName)
{
	var favImg=document.getElementById(id+"_"+columnName+"_del");
	if(favImg.src=="asset://resource/delete.png" || favImg.src=="asset://resource/delete_hover.png")
	{
		cpp._delete(id);
	}	
}
function doConvo(id,column,reply)
{
	var shell=document.getElementById(id+"_"+column+"_replyShell");
	if(shell)
	{
		if(shell.style.display=="none" || shell.style.display=="")
		{
			shell.style.display="block";
			shell.innerHTML="<img src=\"asset://resource/activity.gif\" />";
			cpp.setIdToTweetHtml(id+"_"+column+"_replyShell",reply,id+"_"+column+"_replyShell");
		}
		else
		{
			shell.innerHTML="<img src=\"asset://resource/activity.gif\" />";
			shell.style.display="none";
		}
	}
	/*else
	{
		var n=nodeFromHtml('<div class="replyShell" id="'+id+'_'+column+'_replyShell"><img src="asset://resource/activity.gif" id="activity" /></div>');
		document.getElementById(column).insertBefore(n,document.getElementById(id+"_"+column).nextSibling);
		cpp.setIdToTweetHtml(id+"_"+column+"_replyShell",reply,id+"_"+column+"_replyShell");
	}*/
}
function remove(id)
{	
	var elem=document.getElementById(id);
	if(elem)
		elem.parentNode.removeChild(elem);
}
function addFollower(id)
{
	var c=document.getElementsByClassName('user '+id);
	for(var i=0;i<c.length;)
		c[i].className="followinguser "+id;
}
function toggleFollow(id)
{
	var button=document.getElementById('followbutton');
	if(button)
	{
		if(button.innerHTML=="Follow")
			cpp.follow(id,1);
		else
			cpp.follow(id,0);
	}
	else
		cpp.follow(id,-1);
}
function popup(text,url)
{
var str="<div id='light' style='text-align:center;vertical-align:middle; position:absolute;top:0;left:0;width:100%;height:100%;'> <div style='top:15%; position: absolute;text-align:center;position: relative;text-align: center;max-width: 80%;display:inline-block;max-height: 80%; min-width:70%;'> <div class='white_content' id='popup_content'>    "+text+(url?("<div style='z-index:1005;'><a href='javascript:;' onclick='cpp.openInNativeBrowser(\""+url+"\");' >Open in browser</a>   </div>"):"")+"</div></div>      <div id='fade' class='black_overlay' onclick='remove(\"light\");'></div></div>";
//cpp.debug(str);
document.getElementById('body').insertBefore(nodeFromHtml(str),null);
}

function lightbox(url,url2)
{
	//document.getElementById('body').insertBefore(nodeFromHtml("<div id=\"light\" class=\"white_content\"><object data='"+url+"' style='width: 95%;height: 95%'>tada</object><center><a href='javascript:;' onclick=\"cpp.openInNativeBrowser('"+url+"');\">Open in browser</a></center></div><div id=\"fade\" class=\"black_overlay\" onclick=\"remove('light');remove('fade');\"></div>"),null);
	popup("<object data=\'"+url+"\' class='lightbox_content' type='image/jpeg'>tada</object>",url2);
	}
function mutePop(string)
{
	var body="<div style='margin:50px;' id='mute'>Mute <input type='text'  id='mutestring' size='8' value='"+string+"' onsubmit='cpp.mute(\""+string+"\",document.getElementById(\"muteTime\").value);remove(\"light\");'></input> for <input type='text'  id='muteTime' size='6' value='99999999' onsubmit='cpp.mute(\""+string+"\",document.getElementById(\"muteTime\").value);remove(\"light\");'></input> hours.<br/><br/><button type='button' onclick='remove(\"light\");' style='float:left; vertical-align:bottom;'>Cancel</button><button type='button' onclick='cpp.mute(\""+string+"\",document.getElementById(\"muteTime\").value);remove(\"light\");' style='float:right; vertical-align:bottom;'>OK</button></div>";
		popup(body);
}