function FreeLockPetSave( charaindex,petindex ,pooltype)
	local petId = char.getInt(petindex,"宠ID");
	if pooltype == 1 then--私仓
		for i=1,table.getn(NoPetId1) do
			if petId == NoPetId1[i] then
				char.TalkToCli(charaindex, -1, "该宠物不可存入仓库！", 6);
				return 1;
			end
		end
	elseif pooltype == 2 then--公仓
		for i=1,table.getn(NoPetId2) do
			if petId == NoPetId2[i] then
				char.TalkToCli(charaindex, -1, "该宠物不可存入公用仓库！", 6);
				return 1;
			end
		end
	end
	return 0;
end

--pooltype(1为个人仓库，2为公用仓库)
function FreeLockItemSave( charaindex,itemindex ,pooltype)
	local itemId = item.getInt(itemindex,"序号");
	local itemuse = item.getChar(itemindex,"使用函数名");
	if pooltype == 1 then
		--函数
		for i=1,table.getn(NoItemUse1) do
			if itemuse == NoItemUse1[i] then
				char.TalkToCli(charaindex, -1, "该物品不可存入仓库！", 6);
				return 1;
			end
		end
		--编号
		for i=1,table.getn(NoItemId1) do
			if itemId == NoItemId1[i] then
				char.TalkToCli(charaindex, -1, "该物品不可存入仓库！", 6);
				return 1;
			end
		end
	elseif pooltype == 2 then
		--函数
		for i=1,table.getn(NoItemUse2) do
			if itemuse == NoItemUse2[i] then
				char.TalkToCli(charaindex, -1, "该物品不可存入公用仓库！", 6);
				return 1;
			end
		end
		--编号
		for i=1,table.getn(NoItemId2) do
			if itemId == NoItemId2[i] then
				char.TalkToCli(charaindex, -1, "该物品不可存入公用仓库！", 6);
				return 1;
			end
		end
	end
	return 0;
end

function reload()
	mydata();
end

function mydata()
	--禁止存入私仓函数
	NoItemUse1 = {"ITEM_useSkup"};
	--禁止存入公仓函数
	NoItemUse2 = {"ITEM_useSkup"};
	--禁止存入私仓编号
	NoItemId1 = {18538,18539,18540,18541,18542,23013,23014,23015,23016,23017,23018};
	--禁止存入公仓编号
	NoItemId2 = {18538,18539,18540,18541,18542,23013,23014,23015,23016,23017,23018};
	
	--禁止存入私仓的宠物
	NoPetId1 = {};
	--禁止存入公仓的宠物
	NoPetId2 = {};
end

function main()
	mydata();
	magic.addLUAListFunction("重读存仓数据", "reload", "", 1, "[重读存仓数据]");
end
