--此lua是职业技能使用时进行判断，返回0不可，返回1为可以
function FreeCheckProfessionSkill(charaindex,skillid,Pskillid)
	--print(charaindex,skillid,Pskillid);
	local mymap = char.getInt(charaindex,"地图号"); 
	for i=1,table.getn(NoSkillMap) do
		if mymap == NoSkillMap[i] then
			char.TalkToCli(charaindex, -1, "该地图无法使用职业技能！", "红色");
			return 0;
		end
	end
	return 1;
end

function mydata()
	NoSkillMap = {};--不可使用职业技能的地图
end

function reload(charaindex, data)
	mydata();
end


function main()
	mydata();
	magic.addLUAListFunction("重读限职地图", "reload", "", 1, "[重读限职地图]");
end
