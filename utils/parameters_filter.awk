{ 
if (index($1, "fann_get")) {
	name = substr($1, 10);
	the = index($0, "The");
	if (the == 0)
		the = index($0, "the");
	comment = substr($0, the + 1);
	comment = substr(comment, 0, length(comment) - 1);
	printf("'%s' => array(\n\t'params' => array('%s' => ''),\n\t'comment' => 't%s',\n\t'test_param' => '',\n),\n", name, name, comment); 
}
}
