function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end

function FreeTradePet( charaindex, petindex )
	return 0
end

function FreeTradeItem( charaindex, itemindex )
	if item.getInt(itemindex, "序号") == 22001 then
		local data = item.getChar(itemindex, "字段")--获取item6的字段
		local valiity = other.atoi(data)
		if other.time() >= valiity then
			char.TalkToCli(charaindex, -1, "【错误提示】支票已过期，无法进行交易。", "红色")
			return 1
		end
	end
	return 0
end

function data()

end

function main()
	data()
end

