function upload(){
	closeInfo();
	var fileName=document.getElementById("fileName").value;
	if(fileName=="" || fileName==null){
		alert("请先选择文件进行上传！");
	}else if(document.getElementById("fileName").files[0].size> 541906131){
		alert("您选择的文件太大，无法传送！");
		document.getElementById("fileName").value="";
	}
	else{
		document.getElementById("submit_btn").disabled=true;
		document.getElementById("nickname").value=document.getElementById("fileName").files[0].name;
		var fm = document.getElementById('mainForm');
		var fd = new FormData(fm);

		console.log(fd);
		var xhr = new XMLHttpRequest();
		xhr.open('post','/file');
		var bar = document.getElementById('bar');
		xhr.upload.onprogress = function(evt){
			percent = (evt.loaded / evt.total).toFixed(2);
			bar.style.width = percent*100 + '%';
		}
		xhr.onreadystatechange = function(){
			if(xhr.readyState == 4){
				var obj=JSON.parse(xhr.responseText);
				document.getElementById("fileName").value="";
				document.getElementById("submit_btn").disabled=false;
				if(obj.ok){
					document.getElementById("result").innerHTML="上传成功！"
					document.getElementById("message").innerHTML=obj.msg;
					document.getElementById("container").setAttribute(
						"class","alert alert-dismissable alert-success"
					);
					document.getElementById('info').style.display="block";
				}else{
					document.getElementById("result").innerHTML="上传失败！"
					document.getElementById("message").innerHTML=obj.msg;
					document.getElementById("container").setAttribute(
						"class","alert alert-dismissable alert-danger"
					);
					document.getElementById('info').style.display="block";
				}
			}
		}
		xhr.send(fd);
	}

}
function closeInfo(){
	document.getElementById("bar").style.width="0%";
	document.getElementById('info').style.display="none";
}