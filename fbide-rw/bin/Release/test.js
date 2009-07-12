var arr = [
    "one", "two", "three"
];

function greet (name)
{
    var msg = "";
    for (i in arr)
    {
        msg += arr[i] + " ";
    }
    return msg + name;
}

print (greet("World"));
