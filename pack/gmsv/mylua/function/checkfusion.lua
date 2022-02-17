--此lua是宠物融合时进行判断，返回0不可融合，返回1为通过
function FreeCheckFusion(petindex1,petindex2,petindex3,pettype)
	
	return 1;
end

--此lua是宠物融合时技能遗传进行判断，返回0技能不可遗传，返回1为可遗传
function FreeFusionSkill(charaindex,petskill)
	for i=1,table.getn(NoPetSkill) do
		if NoPetSkill[i] ~= nil then
			if petskill == NoPetSkill[i][1] then
				char.TalkToCli(charaindex, -1, "技能["..NoPetSkill[i][2].."]为不可遗传的技能，所以无法融合！", "红色");
				return 0;
			end
		end
	end
	return 1;
end

function mydata()
	--T一击必杀改,T背水之战之其３
	NoPetSkill = {{41,"T一击必杀改"},
				  {52,"T背水之战之其３"},
				  {600,"暗月变身"},
				  {601,"大地铠甲"},
				  {602,"水灵滋润"},
				  {603,"火焰威能"},
				  {604,"风之吹息"},
				  {614,"栗子连激"},
				  {617,"毒煞蔓延"},
				  {628,"乌力化"},
				  {630,"地灵威能"},
				  {631,"乌力化"},
				  {635,"黑乌力化"},
				  {638,"群蜂乱舞"},
				  {641,"群蜂乱舞改"},
				  };--不可遗传的技能
end


function reload(charaindex, data)
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读融合判断", "reload", "", 1, "[重读融合判断]");
end
