function SetPlayerPet( charindex, petid,lock)
	local petindex = char.AddPet(charindex, petid, 1);
	char.setInt(petindex,"绑定",lock);
end

function FreeCharCreate( charindex,hometown,clifd )
	char.setInt(charindex,"骑乘等级",200);--设置出生骑乘等级
	--给予出生宠物
	if hometown == 4 then
		SetPlayerPet(charindex, 1,0);
	else
		SetPlayerPet(charindex, hometown+1,0);
	end
	--给予水魔
	SetPlayerPet(charindex, 5000,1);
	
	--给予出生道具
	char.Additem( charindex,2237);
	char.Additem( charindex,2238);
	char.Additem( charindex,2239);
	char.Additem( charindex,3401);
	
	local FuLi = 1;
	local cdkey = char.getChar(charindex,"帐号");--取得玩家帐号
	local token = "select FuLi from `CSA_Login` where `Name` = '"..cdkey.."'";
	local ret = sasql.query(token);
	if ret == 1 then
		sasql.free_result();
		sasql.store_result();
		local num = sasql.num_rows();
		if num > 0 then--如果查询信息存在的话，获取信息
			sasql.fetch_row();
			FuLi = other.atoi(sasql.data(1));
		end
	end
	if FuLi == 0 then
		char.Additem( charindex,23012);
		token = "update `CSA_Login` SET FuLi=1 where `Name` = '"..cdkey.."'";
		sasql.query(token);
	end	
	--给予新手石币
	char.setInt(charindex,"石币",100000);
	--给予新手虎
	for i=1,4 do
		for j=1,12 do
			if char.getInt( charindex, "图像号") == MetamoList[i][j] then
				SetPlayerPet(charindex, MetamoList[i][13],1);
				SetPlayerPet(charindex, MetamoList[i][13],1);
			end
		end
	end
end

function mydata()
		MetamoList={
		  --{ 小矮子   赛亚人  辫子男孩  酷哥   熊皮男   大个    小矮妹  熊皮妹  帽子妹  短发夹妹  手套女  辣妹  虎}, 此行为说明行
			{ 100000, 100025, 100055, 100060, 100095, 100100, 100135, 100145, 100165, 100190, 100200, 100230, 2483},	--红
			{ 100005, 100030, 100050, 100065, 100085, 100115, 100120, 100140, 100170, 100195, 100210, 100225, 2481},	--绿
			{ 100010, 100035, 100045, 100070, 100090, 100110, 100125, 100150, 100160, 100185, 100215, 100220, 2484},	--金
			{ 100015, 100020, 100040, 100075, 100080, 100105, 100130, 100155, 100175, 100180, 100205, 100235, 2482}	--黄
		}
end

function main()
	mydata();
end
