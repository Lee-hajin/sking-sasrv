--人物角色删除时触发，主要做sql数据清理用
function FreeCharDelete(clifd,Saveindex,cdkey)
	--delete from staff where id=9;
	local token = "";
	--清除称号数据
	token = "delete from CSA_Title where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex..";";
	sasql.query(token);
	--清除皮肤数据
	token = "delete from CSA_Skin where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex..";";
	sasql.query(token);
	--清除特效
	token = "delete from Data_Halo where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex..";";
	sasql.query(token);
	return ;
end

function mydata()
	
end

function main()
	mydata();
end
