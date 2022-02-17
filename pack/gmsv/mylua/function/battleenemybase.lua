--掉落物品判断，返回0不允许掉落，返回1允许掉落
function battledorpitemcheck(itemid)
	local serverid = config.getServernumber();--取得线路ID
	
	for i=1,table.getn(NoItemId) do
		if itemid == NoItemId[i][1] then
			for j=1,table.getn(NoItemId[i][2]) do
				if NoItemId[i][2][j] == serverid then
					return 0;
				end
			end
		end
	end
	return 1;
end

--生成宠物时进行强制改名
function battlecheanpet(newindex)
	local serverid = config.getServernumber();--取得线路ID
	--print(char.getInt(newindex,"宠ID"),serverid);
	--print(pet.GetEnemyId(newindex));
	--改名
	local petid = char.getInt(newindex,"宠ID");
	for i=1,table.getn(GMPetName) do
		if petid == GMPetName[i][1] then
			for j=1,table.getn(GMPetName[i][3]) do
				if GMPetName[i][3][j] == serverid then
					char.setChar(newindex,"名字",GMPetName[i][2]);
					char.Updata(newindex, "名字");
				end
			end
		end
	end
	return 1;
end

function mydata()
	--禁止掉落物品，禁止线路列表
	NoItemId = {
				};
	--enemybaseid,改为名字,改名线路
	GMPetName = {
				};
end

function reload()
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读掉落设置", "reload", "", 1, "[重读掉落设置]");
end
