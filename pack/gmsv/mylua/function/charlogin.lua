--此lua是登录后计算离线时间，点数最高限制，安全码锁定，机器码读取
function FreeCharLogin( charindex )
	local parameter = {charindex};
	--检查身上宠物
	for i = 0, 4 do
		petindex = char.getCharPet(charindex, i)
		if char.check(petindex) == 1 then
			
		end
	end
	--检查个人仓库宠物
	for i = 0, 14 do
		petindex = char.getCharPoolPet(charindex, i)
		if char.check(petindex) == 1 then
			local petid = char.getInt( petindex, "宠ID");
			if petid == 1385 then
				char.DelPet(charindex,petindex);
				local logstr = os.date("%Y-%m-%d %X",os.time())
						.."|"..char.getChar(charindex, "账号")
						.."|"..char.getChar(charindex, "名字")
						.."|删除个人仓库宠物："..petid
						.."\n";
				local f = assert(io.open("./mylua/log/DelPet/"..os.date("%Y%m%d",os.time())..".log", "a"));
					f:write( logstr);
					f:close();
			end
		end
	end
	--检查公用仓库宠物
	for i = 0, 29 do
		petindex = char.getDepotPetIndex(charindex, i)
		if char.check(petindex) == 1 then
			local petid = char.getInt( petindex, "宠ID");
			if petid == 1385 then
				char.DelPet(charindex,petindex);
				local logstr = os.date("%Y-%m-%d %X",os.time())
						.."|"..char.getChar(charindex, "账号")
						.."|"..char.getChar(charindex, "名字")
						.."|删除公用仓库宠物："..petid
						.."\n";
				local f = assert(io.open("./mylua/log/DelPet/"..os.date("%Y%m%d",os.time())..".log", "a"));
					f:write( logstr);
					f:close();
			end
		end
	end
	--点数上限判断
	if other.CallFunction("RePlayerPoint", "mylua/other/RePlayerPoint.lua", parameter) == 1 then
		char.TalkToCli(charaindex, -1, "你的点数异常，系统自动为你重新调整点数！", 6);
	end
	
	--登录传送地图
	local floorid = char.getInt(charindex, "地图号");
	for i=1,table.getn(unlawthislogut) do
		if floorid == unlawthislogut[i][1] then
			char.WarpToSpecificPoint(charindex,unlawthislogut[i][2][1],unlawthislogut[i][2][2],unlawthislogut[i][2][3]);
			break ;
		end
	end
	
	return 1;
end

function mydata()
	unlawthislogut = {{125, {2000, 65, 58}}
					,{126, {2000, 65, 58}}
					,{127, {2000, 65, 58}}
					,{128, {2000, 65, 58}}
					,{8190, {2000, 65, 58}}
					,{12345, {2000, 65, 58}}
					,{40001, {2000, 65, 58}}
					,{40002, {2000, 65, 58}}
					,{40003, {2000, 65, 58}}
					,{40004, {2000, 65, 58}}
					,{40005, {2000, 65, 58}}
					,{40006, {2000, 65, 58}}
					,{40007, {2000, 65, 58}}
					,{40008, {2000, 65, 58}}
					,{40009, {2000, 65, 58}}
					,{40010, {2000, 65, 58}}
					,{50001, {2000, 65, 58}}
					,{50002, {2000, 65, 58}}
					,{50003, {2000, 65, 58}}
					,{50004, {2000, 65, 58}}
					,{104,   {2000, 65, 58}}
					,{133,   {2000, 83, 80}}
					--,{130,   {2000, 89, 80}}
					,{65501, {2000, 65, 58}}
					,{2015, {2000, 44, 67}}
					};
end

function main()
	mydata();
end
