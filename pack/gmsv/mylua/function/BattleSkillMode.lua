--宠技使用回调(必须有返回值)
--charaindex：玩家索引
--petindex：宠物索引
--petskillindex：宠技索引
--petskillid：宠技编号
--战斗类型:0无战斗 1为PVE 2为PVP 3w为EVE

function FreeBattleSkillMode(charaindex,petindex, petskillindex, petskillid)
	--print(charaindex,petindex, petskillindex, petskillid);
	local battleindex = char.getWorkInt(charaindex,"战斗索引");--取得战斗索引
	local battletype = battle.getInt(battleindex,"类型");
	if battle.checkindex(battleindex) == 1 then
		for i=1,table.getn(PetSkillData) do
			if petskillid == PetSkillData[i][1] then
				for j=1,table.getn(PetSkillData[i][2]) do
					if battletype == PetSkillData[i][2][j] then
						return 1;
					end
				end
			end
		end
	end
	return 0;
end

function mydata()
	--技能ID，在某些战斗类型中禁止
	PetSkillData = {
				   };			 
end


function reload()
	print("重读战斗宠技完成");
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读战斗宠技", "reload", "", 1, "[重读战斗宠技]");
end
